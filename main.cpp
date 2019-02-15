// ----------------------------------------------------------------------------
// Copyright 2016-2018 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------
#ifndef MBED_TEST_MODE

#include "mbed.h"
#include "simple-mbed-cloud-client.h"
#include "FATFileSystem.h"
#include "LittleFileSystem.h"

// An event queue is a very useful structure to debounce information between contexts (e.g. ISR and normal threads)
// This is great because things such as network operations are illegal in ISR, so updating a resource in a button's fall() function is not allowed
EventQueue eventQueue;

// Default network interface object. Don't forget to change the WiFi SSID/password in mbed_app.json if you're using WiFi.
NetworkInterface *net = NetworkInterface::get_default_instance();

// Default block device available on the target board
BlockDevice *bd = BlockDevice::get_default_instance();

#if COMPONENT_SD || COMPONENT_NUSD
// Use FATFileSystem for SD card type blockdevices
FATFileSystem fs("fs", bd);
#else
// Use LittleFileSystem for non-SD block devices to enable wear leveling and other functions
LittleFileSystem fs("fs", bd);
#endif

// Default User button for GET example
InterruptIn button(BUTTON1);
// Default LED to use for PUT/POST example
DigitalOut led(LED1);
// Temperature reading from microcontroller
AnalogIn adc_temp(ADC_TEMP);
// Voltage reference reading from microcontroller
AnalogIn adc_vref(ADC_VREF);

#define SENSORS_POLL_INTERVAL 1.0

// Declaring pointers for access to Pelion Client resources outside of main()
MbedCloudClientResource *res_button;
MbedCloudClientResource *res_led;
MbedCloudClientResource *res_temperature;
MbedCloudClientResource *res_voltage;

// When the device is registered, this variable will be used to access various useful information, like device ID etc.
static const ConnectorClientEndpointInfo* endpointInfo;

/**
 * PUT handler
 * @param resource The resource that triggered the callback
 * @param newValue Updated value for the resource
 */
void led_put_callback(MbedCloudClientResource *resource, m2m::String newValue) {
    printf("PUT received. New value: %s\n", newValue.c_str());
    led = atoi(newValue.c_str());
}

/**
 * POST handler
 * @param resource The resource that triggered the callback
 * @param buffer If a body was passed to the POST function, this contains the data.
 *               Note that the buffer is deallocated after leaving this function, so copy it if you need it longer.
 * @param size Size of the body
 */
void led_post_callback(MbedCloudClientResource *resource, const uint8_t *buffer, uint16_t size) {
    printf("POST received. Payload: %s\n", res_led->get_value().c_str());
    led = atoi(res_led->get_value().c_str());
}

/**
 * Button function triggered by the physical button press.
 */
void button_press() {
    int v = res_button->get_value_int() + 1;
    res_button->set_value(v);
    printf("Button clicked %d times\n", v);
}

/**
 * Notification callback handler
 * @param resource The resource that triggered the callback
 * @param status The delivery status of the notification
 */
void button_callback(MbedCloudClientResource *resource, const NoticationDeliveryStatus status) {
    printf("Button notification, status %s (%d)\n", MbedCloudClientResource::delivery_status_to_string(status), status);
}

/**
 * Registration callback handler
 * @param endpoint Information about the registered endpoint such as the name (so you can find it back in portal)
 */
void registered(const ConnectorClientEndpointInfo *endpoint) {
    printf("Registered to Pelion Device Management. Endpoint Name: %s\n", endpoint->internal_endpoint_name.c_str());
    endpointInfo = endpoint;
}

/**
 * Update sensors and report their values.
 * This function is called periodically.
 */
void sensors_update() {
    float temp = adc_temp.read()*100;
    float vref = adc_vref.read();
    printf("ADC temp:  %6.4f C,  vref: %6.4f %%\r\n", temp, vref);
    if (endpointInfo) {
        res_temperature->set_value(temp);
        res_voltage->set_value(vref);
    }
}


int main(void) {
    printf("\nStarting Simple Pelion Device Management Client example\n");

    // If the User button is pressed ons start, then format storage.
    DigitalIn *user_button = new DigitalIn(BUTTON1);
#if TARGET_DISCO_L475VG_IOT01A
    // The user button on DISCO_L475VG_IOT01A works the other way around
    const int PRESSED = 0;
#else
    const int PRESSED = 1;
#endif
    if (user_button->read() == PRESSED) {
        printf("User button is pushed on start. Formatting the storage...\n");
        int storage_status = fs.reformat(bd);
        if (storage_status != 0) {
            if (bd->erase(0, bd->size()) == 0) {
                if (fs.format(bd) == 0) {
                    storage_status = 0;
                    printf("The storage reformatted successfully.\n");
                }
            }
        }
        if (storage_status != 0) {
            printf("ERROR: Failed to reformat the storage (%d).\n", storage_status);
        }
    } else {
        printf("You can hold the user button during boot to format the storage and change the device identity.\n");
    }

    // Connect to the internet (DHCP is expected to be on)
    printf("Connecting to the network using the default network interface...\n");
    net = NetworkInterface::get_default_instance();

    nsapi_error_t net_status = -1;
    for (int tries = 0; tries < 3; tries++) {
        net_status = net->connect();
        if (net_status == NSAPI_ERROR_OK) {
            break;
        } else {
            printf("Unable to connect to network. Retrying...\n");
        }
    }

    if (net_status != NSAPI_ERROR_OK) {
        printf("ERROR: Connecting to the network failed (%d)!\n", net_status);
        return -1;
    }

    printf("Connected to the network successfully. IP address: %s\n", net->get_ip_address());

    printf("Initializing Pelion Device Management Client...\n");

    // SimpleMbedCloudClient handles registering over LwM2M to Pelion DM
    SimpleMbedCloudClient client(net, bd, &fs);
    int client_status = client.init();
    if (client_status != 0) {
        printf("ERROR: Pelion Client initialization failed (%d)\n", client_status);
        return -1;
    }

    // Creating resources, which can be written or read from the cloud
    res_button = client.create_resource("3200/0/5501", "button_count");
    res_button->set_value(0);
    res_button->methods(M2MMethod::GET);
    res_button->observable(true);
    res_button->attach_notification_callback(button_callback);

    res_led = client.create_resource("3201/0/5853", "led_state");
    res_led->set_value(1);
    res_led->methods(M2MMethod::GET | M2MMethod::PUT);
    res_led->attach_put_callback(led_put_callback);

    // Sensor resources
    res_temperature = client.create_resource("3303/0/5700", "temperature");
    res_temperature->set_value(0);
    res_temperature->methods(M2MMethod::GET);
    res_temperature->observable(true);

    res_voltage = client.create_resource("3316/0/5700", "voltage");
    res_voltage->set_value(0);
    res_voltage->methods(M2MMethod::GET);
    res_voltage->observable(true);

    printf("Initialized Pelion Device Management Client. Registering...\n");

    // Callback that fires when registering is complete
    client.on_registered(&registered);

    // Register with Pelion DM
    client.register_and_connect();

    int i = 600; // wait up 60 seconds before attaching sensors and button events
    while (i-- > 0 && !client.is_client_registered()) {
        wait_ms(100);
    }

    button.fall(eventQueue.event(&button_press));
    printf("Press the user button to increment the LwM2M resource value...\n");

    // The timer fires on an interrupt context, but debounces it to the eventqueue, so it's safe to do network operations
    Ticker timer;
    timer.attach(eventQueue.event(&sensors_update), SENSORS_POLL_INTERVAL);

    // You can easily run the eventQueue in a separate thread if required
    eventQueue.dispatch_forever();
}
#endif
