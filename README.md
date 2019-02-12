##How to use

1. Import the code 'mbed import git@github.com:BlackstoneEngineering/mbed-os-example-e2e-demo.git1
1. Add certs to the device `mbed dm init -d "http://os.mbed.com" --model-name "modelname" -q --force` (make sure you have setup your API key in your environment)
1. Compile annd run on your device `mbed compile --profile=uTensor/build_profile/release.json --flash; mbed sterm -b 115200`

Your code should now be running on the device. Make sure to note your Pelion Endpoint ID that is printed out to the terminal
1. Change the speed of the heartbeat function `t.attach(heartbeat,0.2);`
1. Issue an update ` mbed dm update device -D 0168e01408b300000000000xxxxxxx -m DISCO_F413ZH --profile=uTensor/build_profile/release.json  -vv `
Now you should be able to see the update be issued to the board, then the board reboot and run the new image. I suggest changing the heartbeat speed to have a visual indicator. 


### Troubleshooting
- If your update campaign gets stuck in 'draft' then you need to go to the pelion cloud interface and remove any 'active' campaigns that are blocking your campaign
- Nuke the certs - holding down the 'USER' button then pressing reset. Keep holding user button untill main program starts, this will reset your update certs in the bootloader and reformat your file system for the connect certs

