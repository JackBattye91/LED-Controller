#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Device.h>

enum FEATURES
{
  FEATURE_SINGLE_COLOR = 1,
  FEATURE_SOLID_COLOR = 2,
  FEATURE_MULTI_COLOR = 4
};

// reset arduino function
void(* resetFunc) (void) = 0;

// ssid for devices to connect to ardiuno
const char *ssid = "LED-Controller";
const char *password = "123456";
const int features = FEATURE_SINGLE_COLOR | FEATURE_SOLID_COLOR;

const int BrightnessPin = 3;
const int RedPin = 3;
const int GreenPin = 4;
const int BluePin = 5;

Device device = Device();
device.Values.add(std::pair<String, String>("red", "0"));


// setup server on port 3001
ESP8266WebServer server(3001);
IPAddress ip;

struct { byte red, green, blue, brightness; } color;

void setup()
{
  // setup Serial port
  Serial.begin(115200);

  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);

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
    
      // setup server addresses
      server.on("/get/state", handleGetState);
      server.on("/set/state", handleSetState);
    }
}

void loop()
{
  if (state == 0)
  {
    analogWrite(RedPin, 0);
    analogWrite(GreenPin, 0);
    analogWrite(BluePin, 0);
  }
  else if (state == FEATURE_SINGLE_COLOR)
  {
    analogWrite(BrightnessPin, color.brightness);
  }
  else if (state == FEATURE_SOLID_COLOR)
  {     
    analogWrite(RedPin, (color.red / 255) * color.brightness);
    analogWrite(GreenPin, (color.green / 255) * color.brightness);
    analogWrite(BluePin, (color.blue / 255) * color.brightness);
  }
  
  server.handleClient();
}

void handleGetState()
{
  if (features & FEATURE_SINGLE_COLOR)
  {
    String json = "{";
  
    json += "\t\"UUID\" : '" + String(uuid) + "',\n";
    json += "\t\"Name\" : '" + String(deviceName) + "',\n";
    json += "\t\"State\" : '" + 
    json += "\t\"Features\" : '" + String("1") + "',\n";
    json += "\t\"Values\" : [ \"brightness\" : " + String(color.brightness) + "]\n";
  
    json += "}";
    server.send(200, "application/json", json);
  }
  else if (features & FEATURE_SOLID_COLOR)
  {
    String json = "{";
  
    json += "\t'UUID' : '" + String(uuid) + "',\n";
    json += "\t'Name' : '" + String(deviceName) + "',\n";
    json += "\t'Features' : '" + String("2") + "',\n";
    json += "\t'Color': { 'red' : '" + String(color.red) + "', 'blue' : '" + String(color. blue) + "', 'green' : '" + String(color.green) + "' },\n";
    json += "\t'Brightness' : '" + String(color.brightness) + "'\n";
  
    json += "}";
    server.send(200, "application/json", json);
  }
  else if (features & FEATURE_MULTI_COLOR)
  {
    String json = "{";
  
    json += "\t'UUID' : '" + String(uuid) + "',\n";
    json += "\t'Name' : '" + String(deviceName) + "',\n";
    json += "\t'Features' : '" + String("2") + "',\n";
    json += "\t'Color': [\n";
    json += "\t\t{ 'red' : '" + String(color.red) + "', 'blue' : '" + String(color. blue) + "', 'green' : '" + String(color.green) + "' },\n";
    json += "\t],\n";
    json += "\t'Brightness' : '" + String(color.brightness) + "'\n";
  
    json += "}";
    server.send(200, "application/json", json);
  }
}

void handleSetState()
{
  if (features & FEATURE_SINGLE_COLOR)
  {
    if (server.hasArg("name"))
      deviceName = server.arg("name");
      
    if (server.hasArg("brightness"))
      color.brightness = strToByte(server.arg("brightness"));
  }
  else if (features & FEATURE_SOLID_COLOR)
  {
    if (server.hasArg("name"))
      deviceName = server.arg("name");
    
    if (server.hasArg("red"))
      color.red = strToByte(server.arg("red"));
      if (server.hasArg("blue"))
      color.blue = strToByte(server.arg("blue"));
      if (server.hasArg("green"))
      color.green = strToByte(server.arg("green"));
      
    if (server.hasArg("brightness"))
      color.brightness = strToByte(server.arg("brightness"));
  }
}

byte strToByte(String str)
{
  byte value = 0;
  
  for (int c = 0; c < str.length(); c++)
  {
    value *= 10;
    
    if (str[c] >= '1' && str[c] <= '9')
    {
      value += c - '0';
    }
  }
  
  return value;
}
