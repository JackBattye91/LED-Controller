# LED-Controller
The project is an addressable LED controller that has an REST API for controlling: color, brightness, number of LED, etc.

## Installation ##
### Requirements ###
- Adruino IDE
- D1 Mini or other Wifi enabled arduino platform
- Addressable LEDs

### Arduino ###
You will need to install the IDE along with the FastLED library. The IDE can be downloaded from [here](https://www.arduino.cc/en/software). The FastLED library can be found in the Library Manager (Sketch->Include Library->Manage Libraries).

The arduino script needs to be uploaded to a micro-controller. Todo this start the IDE and open the project file. You will need to configure the IDE to the board you are using. Once this is done click upload.

### Setup ###
When the micro-controller startups it will create a Access Point which you connect to via Wifi. 
