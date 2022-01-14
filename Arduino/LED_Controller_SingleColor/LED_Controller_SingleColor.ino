#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Device.h"

enum FEATURE_FLAGS
{
    FEATURE_NONE = 0,
    FEATURE_ONOFF = 1,
    FEATURE_SINGLECOLOR = 2,
    FEATURE_SOLIDCOLOR = 4,
    FEATURE_MULTICOLOR = 8
};

// reset arduino function
void(* resetFunc) (void) = 0;

// ssid for devices to connect to ardiuno
const char *ssid = "SSID";
const char *password = "PA55W0RD";

const int BrightnessPin = 3;
const int RedPin = 3;
const int GreenPin = 4;
const int BluePin = 5;

Device device;

// setup server on port 3001
ESP8266WebServer server(3001);
IPAddress ip;

void setup()
{
  device = Device();
  device.FeatureFlags |= FEATURE_FLAGS::FEATURE_SINGLECOLOR;
  device.IntValues["red"] = 0;
  device.IntValues["green"] = 255;
  device.IntValues["blue"] = 255;
  device.IntValues["brightness"] = 255;

  
  // setup Serial port
  Serial.begin(115200);

  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);

  // connect to wifi
    byte attempts = 0;
    WiFi.hostname("LED Controller");
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
      server.on("/", handleGetState);
      server.on("/get/state", handleGetState);
      server.on("/set/state", handleSetState);

      Serial.println(ip);
    }
}

void loop()
{
  if (device.FeatureFlags == 0)
  {
    analogWrite(RedPin, 0);
    analogWrite(GreenPin, 0);
    analogWrite(BluePin, 0);
  }
  else if (device.FeatureFlags & FEATURE_FLAGS::FEATURE_SINGLECOLOR > 0)
  {
    analogWrite(BrightnessPin, device.IntValues["brightness"]);
  }
  else if (device.FeatureFlags & FEATURE_SOLIDCOLOR > 0)
  {
    analogWrite(RedPin, device.IntValues["red"] * device.IntValues["brightness"]);
    analogWrite(GreenPin, device.IntValues["green"] * device.IntValues["brightness"]);
    analogWrite(BluePin, device.IntValues["blue"] * device.IntValues["brightness"]);
  }
  
  server.handleClient();
}

void handleGetState()
{
    server.send(200, "application/json", device.ToString().c_str());
}

void handleSetState()
{
  if (server.hasArg("data"))
    device.Parse(server.arg("data").c_str());
}
