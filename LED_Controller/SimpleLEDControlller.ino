#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "Filter.h"

// pins
#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB

// states
#define STATE_OFF                 0
#define STATE_SOLIDCOLOR          1
#define STATE_GRADIENTCOLORS16    2
#define STATE_GRADIENTCOLORS32    3
#define STATE_SETUP               10

// static EEPROM locations
#define EEPROM_STATE              0     // 1 byte
#define EEPROM_NUMLEDS            1     // 1 byte
#define EEPROM_SSID               10    // 50 bytes
#define EEPROM_PASSKEY            60    // 50 bytes
#define EEPROM_BRIGHTNESS         110   // 1 byte
#define EEPROM_SOLIDCOLOR         111   // 3 bytes
#define EEPROM_GRADIENTCOLORS16   114   // 48 bytes
#define EEPROM_GRADIENTCOLORS32   162   // 96 bytes

// reset arduino function
void(* resetFunc) (void) = 0;

// ssid for devices to connect to ardiuno
const char *localSSID = "LED-Controller";
const char *localPassword = "123456";

// stores values in hex
const char HexValues[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
const String StateText[] = { "Off", "Solid Color", "Gradient Color 16", "Gradient Color 32", "Setup" };

// variables
byte state = 0;
byte brightness = 255;

// setup server on port 80
ESP8266WebServer server(80);
IPAddress ip;

// **************************************************** SETUP ****************************************************
void setup() {  
  // setup Serial port
  Serial.begin(115200);

  // setup memory
  EEPROM.begin(512);

  // get stored ssid
  String ssid = readEEPROM(EEPROM_SSID);
  // get stored password
  String password= readEEPROM(EEPROM_PASSKEY);

  // if ssid is not set then enter setup mode
  if (ssid.length() == 0)
  {
    // setup Access Point
    WiFi.softAP(localSSID, localPassword);

    server.on("/", handleRoot);
    server.on("/setup", handleSetWifi);

    state = STATE_SETUP;
  }
  else
  {
    // connect to wifi
    byte attempts = 0;
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED && attempts < 5)
    {
      delay(500);
    }

    // if connected to wifi
    if (WiFi.status() == WL_CONNECTED)
    {
      ip = WiFi.localIP();
      
      // load previous state
      state = EEPROM.read(EEPROM_STATE);
      brightness = EEPROM.read(EEPROM_BRIGHTNESS);
      solidColor = readEEPROMColor(EEPROM_SOLIDCOLOR);
      currentPalette16 = readEEPROMPalette16(EEPROM_GRADIENTCOLORS16);
      currentPalette32 = readEEPROMPalette32(EEPROM_GRADIENTCOLORS32);
    
      // setup server addresses
      server.on("/", handleRoot);
      server.on("/reset", handleReset);
      server.on("/factoryreset", handleFactoryReset);
  
      server.on("/get/state", handleGetState);
      server.on("/get/solidcolor", handleGetSolidColor);
      server.on("/get/brightness", handleGetBrightness);
  
      server.on("/set/off", off);
      server.on("/set/state", handleSetState);
      server.on("/set/solidcolor", handleSetSolidColor);
      server.on("/set/brightness", handleSetBrightness);
  
      server.on("/set/wifi", handleSetWifi);
    }
    // if unable to connect to wifi
    else
    {
      // setup Access Point
      WiFi.softAP(localSSID, localPassword);
      
      server.on("/", handleSetWifi);
    }
  }

  // active server
  server.begin();
}
// **************************************************** LOOP ****************************************************
void loop() {
  // put your main code here, to run repeatedly:

  // change state
  switch(state)
  {
    case STATE_SOLIDCOLOR:
      break;
      
    case STATE_GRADIENTCOLORS16:
      break;

    case STATE_GRADIENTCOLORS32:
      break;
      
    default:
      break;
  }
  
  // update server
  server.handleClient();
}

// **************************************************** SERVER HANDLES ****************************************************
void handleRoot() {
  String html = "";

  html += "Mode: " + StateText[state] + "\n";
  html += "LED #: " + String(numLEDs) + "\n";
  html += "Solid Color: " + colorToString(solidColor) + "\n";
  html += "Gradient 16 Colors: ";
  for (int c = 0; c < 16; c++)
  {
    html += colorToString(currentPalette16[c]);

    if (c != 15)
      html += ", ";
    else
      html += "\n";
  }
  html += "Gradient 32 Colors: ";
  for (int c = 0; c < 32; c++)
  {
    html += colorToString(currentPalette32[c]);

    if (c != 31)
      html += ", ";
    else
      html += "\n";
  }
  html += "IP: " + String(ip[0]) + ":" + String(ip[1]) + ":" + String(ip[2]) + ":" + String(ip[3]) + "\n";
  
  server.send(200, "text/text", html);
}
void handleReset()
{
  resetFunc();
}
void handleFactoryReset()
{
  // clear EEPROM
  for (int e = 0; e < 512; e++)
    EEPROM.write(e, 0);

  // set defaults
  EEPROM.write(EEPROM_STATE, STATE_SOLIDCOLOR);
  writeEEPROMColor(EEPROM_SOLIDCOLOR, CRGB(255,255,255));
  writeEEPROMPalette16(EEPROM_GRADIENTCOLORS16, Rainbow_gp);

  // reset
  resetFunc();
}

void handleGetState()
{
  server.send(200, "text/text", "" + state);
}
void handleGetSolidColor()
{
  server.send(200, "text/text", colorToString(solidColor));
}

void handleGetBrightness()
{
  String strValue = "0x";
  strValue += HexValues[brightness >> 4];
  strValue += HexValues[brightness & 0x0F];

  server.send(200, "text/text", strValue);
}

void off()
{
  for (int l = 0; l < numLEDs; l++)
  {
    leds[l] = CRGB::Black;
  }
  FastLED.show();
}
void handleSetState()
{
  if (server.hasArg("state"))
  {
    state = strToValue(server.arg("state"));

    switch(state)
    {
      case STATE_OFF:
        off();
        break;

      case STATE_SOLIDCOLOR:
        setSolidColor();
        break;
        
      case STATE_GRADIENTCOLORS16:
        setGradientColors16();
        break;

      case STATE_GRADIENTCOLORS32:
        setGradientColors32();
        break;
    }
  }
}
void handleSetWifi()
{
  if (!server.hasArg("ssid") || !server.hasArg("passkey"))
  {
    server.send(200, "text/html", "No data");
    return;
  }
  
  byte attempts = 0;

  WiFi.begin(server.arg("ssid"), server.arg("passkey"));

  while(WiFi.status() != WL_CONNECTED && attempts < 4)
  {
    delay(500);
    attempts++;
  }

  // on successful connection save ssid and password
  if (WiFi.status() == WL_CONNECTED)
  {
    byte ssidLength = writeEEPROM(EEPROM_SSID, server.arg("ssid"));
    byte passkeyLength = writeEEPROM(EEPROM_SSID + ssidLength, server.arg("passkey"));

    String html = "Connected\n";

    // display MAC address
    for (int m = 0; m < 6; m++)
    {
      html += HexValues[WiFi.macAddress()[m] >> 4];
      html += HexValues[WiFi.macAddress()[m] & 0x0F];
      
      if (m < 5)
        html += ":";
      else
        html += "\n";
    }
    
    server.send(200, "text/text", html);
    resetFunc();
  }
  else
    server.send(200, "text/html", "Unable to connect");
}
void handleSetSolidColor()
{
  if (server.hasArg("color"))
  {
    Serial.println("Set Solid Color: " + server.arg("color"));
    solidColor = strToColor(server.arg("color"));
    setSolidColor();
  }
  else if (server.hasArg("h") && server.hasArg("s") && server.hasArg("v"))
  {
    CHSV hsvColor = CHSV(strToValue(server.arg("h")), strToValue(server.arg("s")), strToValue(server.arg("v")));
    hsv2rgb_rainbow(hsvColor, solidColor);
    setSolidColor();
  }
}
void handleSetBrightness()
{
  if (server.hasArg("brightness"))
  {
    
  }
}

// **************************************************** FAST LED ****************************************************
void setSolidColor()
{


  writeEEPROMColor(EEPROM_SOLIDCOLOR, solidColor);
}
void setGradientColors16()
{
  // display gradient colors
  byte colorIndex = 0;
  for (int l = 0; l < numLEDs; l++)      
  {
    // calc colorIndex
    colorIndex = (16 / numLEDs) * l;
    
    // set LED colors
    leds[l] = ColorFromPalette(currentPalette16, colorIndex, brightness, currentBlending);
  }
  FastLED.show();
}
void setGradientColors32()
{
  // display gradient colors
  byte colorIndex = 0;
  for (int l = 0; l < numLEDs; l++)      
  {
    // calc colorIndex
    colorIndex = (32 / numLEDs) * l;
    
    // set LED colors
    leds[l] = ColorFromPalette(currentPalette32, colorIndex, brightness, currentBlending);
  }
  FastLED.show();
}
void setBrightness(byte b)
{
  // set brightness
  brightness = b;
  FastLED.setBrightness(brightness);
}

// **************************************************** EEPROM ****************************************************
// reads a string from EEPROM
String readEEPROM(int addr)
{
  // get length
  int strLength = EEPROM.read(addr);

  // create new string
  String str = String();

  // copy data over
  for(int c = 0; c < strLength; c++)
  {
    str += EEPROM.read(addr + 1 + c);
  }

  // return string length
  return str;
}
// reads a color from EEPROM
CRGB readEEPROMColor(int addr)
{
  return CRGB(EEPROM.read(addr), EEPROM.read(addr + 1), EEPROM.read(addr + 2));
}
CRGBPalette16 readEEPROMPalette16(int addr)
{
  CRGBPalette16 palette;
  for (int c = 0; c < 16; c++)
  {
    palette[c] = readEEPROMColor(addr + (c * 3));
  }
  return palette;
}
CRGBPalette32 readEEPROMPalette32(int addr)
{
  CRGBPalette32 palette;
  for (int c = 0; c < 32; c++)
  {
    palette[c] = readEEPROMColor(addr + (c * 3));
  }
  return palette;
}

// writes a string to EEPROM
int writeEEPROM(int addr, String data)
{
  // write length and count length
  int d = 0; 
  while(data[d] != 0)
  {
    EEPROM.write(addr + 1 + d, data[d]);
    d++;
  }

  // write length to beginning
  EEPROM.write(addr, d);

  // return data length
  return d;
}
// writes a color to the EEPROM
int writeEEPROMColor(int addr, CRGB color)
{
  EEPROM.write(addr, color[0]);
  EEPROM.write(addr + 1, color[1]);
  EEPROM.write(addr + 2, color[2]);
  
  return 3;
}
int writeEEPROMPalette16(int addr, CRGBPalette16 palette)
{
  for (int c = 0; c < 16; c++)
  {
    writeEEPROMColor(addr + (c * 3), palette[c]);
  }
  
  return (16 * 3);
}
int writeEEPROMPalette32(int addr, CRGBPalette32 palette)
{
  for (int c = 0; c < 32; c++)
  {
    writeEEPROMColor(addr + (c * 3), palette[c]);
  }
  
  return (32 * 3);
}

// **************************************************** MISC ****************************************************
// return a color from a HEX string eg #abc123
CRGB strToColor(String str)
{
  CRGB value = CRGB(0);

  // set RED
  value.r = charToValue(str[1]);
  value.r << 4;
  value.r += charToValue(str[2]);

  // set GREEN
  value.g = charToValue(str[3]);
  value.g << 4;
  value.g += charToValue(str[4]);

  // set BLUE
  value.b = charToValue(str[5]);
  value.b << 4;
  value.b += charToValue(str[6]);

  return value;
}

// return the literal value of a character
byte charToValue(char c)
{
  if (c <= '9' && c >= '0')
    return (c - '0');
  if (c >= 'a' && c <= 'f')
    return (c - 'a' + 10);
  if (c >= 'A' && c <= 'F')
    return (c - 'A' + 10);
    
  return 0;
}

// return the number value as a integer 
int strToValue(String str)
{
  int value = 0;
  
  for (int c = 0; c < str.length(); c++)
  {
    if (str[c] >= '1' && str[c] <= '9')
    {
      value *= 10;
      value += c - '0';
    }
  }
  
  return value;
}

// returns the color as a HEX string eg #ff88aa
String colorToString(CRGB color)
{
  String strValue = "#";

  for (int c = 0; c < 3; c++)
  {
    strValue += HexValues[color[c] >> 4];
    strValue += HexValues[color[c] & 0x0F];
  }

  return strValue;
}