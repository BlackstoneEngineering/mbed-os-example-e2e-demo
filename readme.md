## What is this? 
This program is a demonstration of how to take sensor readings on an Mbed OS board, then send those readings to Treasure Data and accept firmware updates using the Pelion Update Service. It is designed to be used in conjuntion with this repo: https://github.com/ARMmbed/E2E-demo 

##How to use

### Initial Program
1. Import the code `mbed import git@github.com:BlackstoneEngineering/mbed-os-example-e2e-demo.git`
1. `cd mbed-os-example-e2e-demo`
1. Add certs to the device `mbed dm init -d "http://os.mbed.com" --model-name "modelname" -q --force` (make sure you have setup your API key in your environment)
1. Compile annd run on your device `mbed compile --profile=uTensor/build_profile/release.json --flash; mbed sterm -b 115200`

### Update Firmware via Pelion Update Service
Your code should now be running on the device. Make sure to note your Pelion Endpoint ID that is printed out to the terminal
1. Change the speed of the heartbeat function `t.attach(heartbeat,0.2);`
1. Recompile the binary (but DO NOT FLASH it) `mbed compile --profile=uTensor/build_profile/release.json`. 
1. Issue an update ` mbed dm update device -D 0168e01408b300000000000xxxxxxx -m DISCO_F413ZH --profile=uTensor/build_profile/release.json  -vv ` You will need to replace the string after `-D` with the device ID of the board you want to update. 
Now you should be able to see the update be issued to the board, then the board reboot and run the new image. I suggest changing the heartbeat speed to have a visual indicator. 


### Troubleshooting
- If your update campaign gets stuck in 'draft' then you need to go to the pelion cloud interface and delete any 'active' campaigns that are blocking your campaign
- Nuke the certs - holding down the 'USER' button then pressing reset. Keep holding user button untill main program starts, this will reset your update certs in the bootloader and reformat your file system for the connect certs
- HTTPS POST errors (anything but 200) - double check your WiFi SSID / Password are set correctly in `mbed_app.json`. 
