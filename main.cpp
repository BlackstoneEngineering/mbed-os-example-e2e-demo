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
// #include "FATFileSystem.h"
#include "qspi_api.h"
#include "QSPI.h"
#include "QSPIFBlockDevice.h"
#include "LittleFileSystem.h"
#include "wifi-ism43362/ISM43362Interface.h"
#include "mbed-os-treasuredata-rest/treasure-data-rest.h"
#include "models/lr_model.hpp"
#include "models/iris_model.hpp"

// #define ENABLE_SENSORS

#ifdef ENABLE_SENSORS
// Workaround for compile error
// SPI is defined in VL53L0X_i2c_platform.h
#ifdef SPI
#undef SPI
#endif
#include "HTS221Sensor.h"
#endif /* ENABLE_SENSORS */

#ifndef ALGO
    #define ALGO 25.0f
#endif

#define BUFF_SIZE   200
char td_buff [BUFF_SIZE];
#define MBED_CONF_APP_TD_API_KEY "10389/599f0416d91cfba4fe1d950e3497e01be258efec"

// An event queue is a very useful structure to debounce information between contexts (e.g. ISR and normal threads)
// This is great because things such as network operations are illegal in ISR, so updating a resource in a button's fall() function is not allowed
EventQueue eventQueue;
Thread thread1;

// Default network interface object
NetworkInterface *net;

// Default block device
// BlockDevice* bd = BlockDevice::get_default_instance();
// FATFileSystem fs("sd", bd);
QSPIFBlockDevice bd(QSPI_FLASH1_IO0, QSPI_FLASH1_IO1, QSPI_FLASH1_IO2, QSPI_FLASH1_IO3,QSPI_FLASH1_SCK,QSPI_FLASH1_CSN,0,8000000);
SlicingBlockDevice sd(&bd,0,(1024*1024*2));
LittleFileSystem fs("sd");
// FATFileSystem fs("sd");

// Declaring pointers for access to Pelion Client resources outside of main()
MbedCloudClientResource *button_res;
// MbedCloudClientResource *pattern_res;
#ifdef ENABLE_SENSORS
// MbedCloudClientResource *distance_res;
// MbedCloudClientResource *temperature_res;
// MbedCloudClientResource *humidity_res;
#endif /* ENABLE_SENSORS */

// This function gets triggered by the timer. It's easy to replace it by an InterruptIn and fall() mode on a real button
void button_press() {
    int v = button_res->get_value_int() + 1;

    button_res->set_value(v);

    printf("Button clicked %d times\n", v);
}

DigitalOut led(LED1);
Ticker t;

void heartbeat(){
    led = !led;
}

void run_ml(){
    printf("Starting run_ml()\r\n");

    Tensor* data = new RamTensor<float>();
    std::vector<uint32_t> input_shape({1, 1 * 4});
    data->init(input_shape);
    //buff.copyTo(tmp);
    float* data_ptr = (float*) data->write<float>(0, 0);
    data_ptr[0] = 7.0;
    data_ptr[1] = 3.2;
    data_ptr[2] = 4.7;
    data_ptr[3] = 1.4;
    
    Context ctx;
    get_iris_model_ctx(ctx, data);
    ctx.eval();
    S_TENSOR prediction = ctx.get({"y_pred:0"});
    printf("Call iris model!!!.\n");
    int result = *(prediction->read<int>(0,0));
    //float result = *(prediction->read<float>(0,0));
    printf("Result is here!!!.\n");
    printf("result: %d\n", result);
    
    Tensor* data_lr = new RamTensor<float>();
    std::vector<uint32_t> input_shape_lr({1, 1 * 2});
    data_lr->init(input_shape_lr);
    //buff.copyTo(tmp);
    float* data_ptr_lr = (float*) data_lr->write<float>(0, 0);
    data_ptr_lr[0] = 3.0;
    data_ptr_lr[1] = 2.0;
    
    Context ctx_lr;
    get_lr_model_ctx(ctx_lr, data_lr);
    ctx_lr.eval();
    S_TENSOR prediction_lr = ctx_lr.get({"y_pred:0"});
    printf("Call lr model!!!.\n");
    float result_lr = *(prediction_lr->read<float>(0,0));
    printf("Result is here!!!.\n");
    printf("result: %f\n", result_lr);

    // delete data;
    return;
}

#ifdef ENABLE_SENSORS
static DevI2C devI2c(PB_11,PB_10);
static DigitalOut shutdown_pin(PC_6);
static HTS221Sensor hum_temp(&devI2c);



void update_sensors() {
        printf("\r\nStarting Update Sensors\r\n");

    // Distance sensor
    // uint32_t distance;
    // int status = range.get_distance(&distance);
    // if (status == VL53L0X_ERROR_NONE) {
    //     distance_res->set_value((int)distance);
    //     printf("VL53L0X [mm]:            %6ld\n", distance);
    // } else {
    //     printf("VL53L0X [mm]:                --\n");
    // }    

    // Temperature sensor
    float temperature = 0.0;
    if(hum_temp.get_temperature(&temperature) == 0) {
        printf("Temperature is %f, Algo is %f, sending to TD.\n", temperature,ALGO);
        // temperature_res->set_value(temperature);
        int flag = -1;
        if(temperature > ALGO){
            flag = 1;
        }else{
            flag = 0;
        }
        {
        int x = 0;
        x=sprintf(td_buff,"{\"temp\":%f,\"flag\":%d}",temperature,flag);
            td_buff[x]=0; // null terminate the string
            TreasureData_RESTAPI* td = new TreasureData_RESTAPI(net,"test_database","test_table", MBED_CONF_APP_TD_API_KEY);
            td->sendData(td_buff,strlen(td_buff));
            delete td;
        }
    } else {
        printf("Error: failed to read temperature.\n");
    }



    // // Humidity sensor
    // float humidity = 0.0;
    // if(hum_temp.get_humidity(&humidity) == 0) {
    //     printf("Humidity is %f %%.\n", humidity);
    //     humidity_res->set_value(humidity);
    // } else {
    //     printf("Error: failed to read humidity.\n");
    // }

    // Output an empty line for visibility
    printf("\n");
    return;
}
#endif /* ENABLE_SENSORS */

#ifndef ENABLE_SENSORS
void update_sensors() {
    printf("\r\nStarting Update Sensors\r\n");

    float temperature = 0;
    int flag = 0;
    {
    int x = 0;
    x=sprintf(td_buff,"{\"temp\":%f,\"flag\":%d}",temperature,flag);
        td_buff[x]=0; // null terminate the string
        TreasureData_RESTAPI* td = new TreasureData_RESTAPI(net,"test_database","test_table", MBED_CONF_APP_TD_API_KEY);
        td->sendData(td_buff,strlen(td_buff));
        delete td;
    }


}

#endif

/**
 * PUT handler
 * @param resource The resource that triggered the callback
 * @param newValue Updated value for the resource
 */
// void pattern_updated(MbedCloudClientResource *resource, m2m::String newValue) {
//     printf("PUT received, new value: %s\n", newValue.c_str());
// }

// /**
//  * POST handler
//  * @param resource The resource that triggered the callback
//  * @param buffer If a body was passed to the POST function, this contains the data.
//  *               Note that the buffer is deallocated after leaving this function, so copy it if you need it longer.
//  * @param size Size of the body
//  */
// void blink_callback(MbedCloudClientResource *resource, const uint8_t *buffer, uint16_t size) {
//     printf("POST received. Going to blink LED pattern: %s\n", pattern_res->get_value().c_str());

//     static DigitalOut augmentedLed(LED1); // LED that is used for blinking the pattern

//     // Parse the pattern string, and toggle the LED in that pattern
//     string s = std::string(pattern_res->get_value().c_str());
//     size_t i = 0;
//     size_t pos = s.find(':');
//     while (pos != string::npos) {
//         wait_ms(atoi(s.substr(i, pos - i).c_str()));
//         augmentedLed = !augmentedLed;

//         i = ++pos;
//         pos = s.find(':', pos);

//         if (pos == string::npos) {
//             wait_ms(atoi(s.substr(i, s.length()).c_str()));
//             augmentedLed = !augmentedLed;
//         }
//     }
// }

/**
 * Notification callback handler
 * @param resource The resource that triggered the callback
 * @param status The delivery status of the notification
 */
// void button_callback(MbedCloudClientResource *resource, const NoticationDeliveryStatus status) {
//     printf("Button notification, status %s (%d)\n", MbedCloudClientResource::delivery_status_to_string(status), status);
// }

/**
 * Registration callback handler
 * @param endpoint Information about the registered endpoint such as the name (so you can find it back in portal)
 */
void registered(const ConnectorClientEndpointInfo *endpoint) {
    printf("***** \r\n *** Connected to Pelion Device Management. Endpoint Name: %s\r\n***\n", endpoint->internal_endpoint_name.c_str());
}

int main(void) {
    printf("Starting Simple Pelion Device Management Client example\n");
    printf("Value of TD ALGO is %f, %d\n",ALGO,ALGO);

    printf("Checking Storage is Formatted\r\n");
    int err = fs.mount(&sd);
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        printf("No filesystem found, formatting... ");
        fflush(stdout);
        err = fs.reformat(&sd);
        printf("%s\n", (err ? "Fail :(" : "OK"));
        if (err) {
            error("error: %s (%d)\n", strerror(-err), err);
        }
    }

    // start heartbeat
    // t.attach(heartbeat,0.2);
    t.attach(heartbeat,2);


    // If the USER button is pushed at launch, format the SD card.
    const int PUSHED = 1;
    DigitalIn *user_button = new DigitalIn(USER_BUTTON);
    if(user_button->read() == PUSHED) {
        printf("USER button is pushed. Formatting the storage...\n");
        if(fs.reformat(&sd) == 0){
            printf("The storage reformatted successfully.\n");
        } else {
            printf("Failed to reformat the storage.\n");
        }
    }

#ifdef ENABLE_SENSORS
    // range.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    hum_temp.init(NULL);
    hum_temp.enable();
#endif /* ENABLE_SENSORS */

    printf("Connecting to the network using Wifi...\n");

    // Connect to the internet (DHCP is expected to be on)
    net = NetworkInterface::get_default_instance();

    nsapi_error_t status = net->connect();

    if (status != NSAPI_ERROR_OK) {
        printf("Connecting to the network failed %d!\n", status);
        return -1;
    }

    printf("Connected to the network successfully. IP address: %s\n", net->get_ip_address());

    
    // SimpleMbedCloudClient handles registering over LwM2M to Pelion DM
    SimpleMbedCloudClient client(net, &bd, &fs);
    int client_status = client.init();
    if (client_status != 0) {
        printf("Pelion Client initialization failed (%d)\n", client_status);
        return -1;
    }

//     // Creating resources, which can be written or read from the cloud
//     button_res = client.create_resource("3200/0/5501", "button_count");
//     button_res->set_value(0);
//     button_res->methods(M2MMethod::GET);
//     button_res->observable(true);
//     button_res->attach_notification_callback(button_callback);

// #ifdef ENABLE_SENSORS
//     distance_res = client.create_resource("3330/0/5700", "distance");
//     distance_res->set_value(0);
//     distance_res->methods(M2MMethod::GET);
//     distance_res->observable(true);

//     temperature_res = client.create_resource("3303/0/5700", "temperature");
//     temperature_res->set_value(0);
//     temperature_res->methods(M2MMethod::GET);
//     temperature_res->observable(true);

//     humidity_res = client.create_resource("3304/0/5700", "humidity");
//     humidity_res->set_value(0);
//     humidity_res->methods(M2MMethod::GET);
//     humidity_res->observable(true);
// #endif /* ENABLE_SENSORS */

//     pattern_res = client.create_resource("3201/0/5853", "blink_pattern");
//     pattern_res->set_value("500:500:500:500:500:500:500:500");
//     pattern_res->methods(M2MMethod::GET | M2MMethod::PUT);
//     pattern_res->attach_put_callback(pattern_updated);

//     MbedCloudClientResource *blink_res = client.create_resource("3201/0/5850", "blink_action");
//     blink_res->methods(M2MMethod::POST);
//     blink_res->attach_post_callback(blink_callback);

    printf("Initialized Pelion Client. Registering...\n");

    // Callback that fires when registering is complete
    client.on_registered(&registered);

    // Register with Pelion DM
    client.register_and_connect();

    // Placeholder for callback to update local resource when GET comes.
    // The timer fires on an interrupt context, but debounces it to the eventqueue, so it's safe to do network operations
    // InterruptIn userButton(USER_BUTTON);
    // userButton.fall(eventQueue.event(button_press));



// #ifdef ENABLE_SENSORS
    Ticker timer1;
    Ticker timer2;
    // timer1.attach(eventQueue.event(update_sensors), 20.0);
    // timer2.attach(eventQueue.event(run_ml), 33.0);
// #endif  ENABLE_SENSORS 


    // You can easily run the eventQueue in a separate thread if required
    // eventQueue.dispatch_forever();
    thread1.start(callback(&eventQueue, &EventQueue::dispatch_forever));

    while(1){
        wait_ms(100);
    }
}
#endif
