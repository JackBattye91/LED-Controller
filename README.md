# LED-Controller
Arduino based addressable LED controller

## Arduino
The arduino code runs a web server that is used to update the LED's state. Depending on how the device is setup and what type of LED's are connected, the color and brightness can be changed.

### First run
When the device is first powered up it will enter a setup mode. You must use the app to connect to the device and input the wifi details. On successful connection the device will restart and enter normal mode.

### Normal running
The device will update the LED 

## Mobile App
The mobile app begins by searching the local network for compatible devices, when one is found it will be displayed in a list. From here users can click on the name of the device to view and update the settings.

