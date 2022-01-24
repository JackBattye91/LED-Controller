#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <EEPROM.h>
#include "../Device.h"

#define EEPROM_SSID       0
#define EEPROM_PASSWORD   32

#define BRIGHTNESS_PIN 3
#define RED_PIN        3
#define GREEN_PIN      4
#define BLUE_PIN       5
#define DATA_PIN       3

#define LED_TYPE      WS2811
#define COLOR_ORDER   RGB1
#define NUM_LEDS      1

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

// wifi settings
std::string ssid = "SSID";
std::string password = "password";

Device device;

// setup server on port 3001
ESP8266WebServer server(3001);
IPAddress ip;

CRGB *leds;
CRGB solidColor;
CRGBPalette32 currentPalett;

void setup()
{
  device = Device(FEATURE_FLAGS::FEATURE_SINGLECOLOR);

  // setup Serial port
  Serial.begin(115200);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  if (ssid.find("SSID") == 0)
  {
    const char *softSSID = "LED_Controller";
    const char *softPassword = "_LED_PASSWORD_4";
    WiFi.softAP(softSSID, softPassword, 5, true);

    server.on("/", handleGetState);
    server.on("/ssid", handleUpdateSSID);
  }
  else
  {
    // connect to wifi
    byte attempts = 0;
    WiFi.hostname(device.Name.c_str());
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED && attempts < 5)
    {
      delay(500);
      attempts++;
    }

    // if connected to wifi
    if (WiFi.status() == WL_CONNECTED)
    {
      ip = WiFi.localIP();
    
      // setup server addresses
      server.on("/", handleGetState);
      server.on("/get/state", handleGetState);
      server.on("/set/state", handleSetState);

      Serial.print("IP: ");
      Serial.println(ip);
    }
  }

  if (device.FeatureFlags == FEATURE_FLAGS::FEATURE_MULTICOLOR)
  {
    leds = new CRGB[NUM_LEDS];
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  }
}

void loop()
{
  if (device.FeatureFlags == 0)
  {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    analogWrite(BRIGHTNESS_PIN, 0);
  }
  else if (device.FeatureFlags & FEATURE_FLAGS::FEATURE_SINGLECOLOR > 0)
  {
    analogWrite(BRIGHTNESS_PIN, device.IntValues["brightness"]);
  }
  else if (device.FeatureFlags & FEATURE_FLAGS::FEATURE_SOLIDCOLOR > 0)
  {
    analogWrite(RED_PIN, device.IntValues["red"] * device.IntValues["brightness"]);
    analogWrite(GREEN_PIN, device.IntValues["green"] * device.IntValues["brightness"]);
    analogWrite(BLUE_PIN, device.IntValues["blue"] * device.IntValues["brightness"]);
  }
  else if (device.FeatureFlags & FEATURE_FLAGS::FEATURE_MULTICOLOR > 0)
  {
    CRGB startColor = CRGB(device.IntValues["startRed"], device.IntValues["startGreen"], device.IntValues["startBlue"]);
    CRGB finishColor = CRGB(device.IntValues["finishRed"], device.IntValues["finishGreen"], device.IntValues["finishBlue"]);
    FastLED.setBrightness(device.IntValues["brightness"]);

    fill_gradient(currentPalett, 0, startColor, NUM_LEDS, finishColor);
    FastLED.show();
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

void handleUpdateSSID()
{
  if (server.hasArg("ssid") && server.hasArg("password"))
  {
    ssid = std::string(server.arg("ssid").c_str());
    password = std::string(server.arg("password").c_str());

    byte attempts = 0;
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED && attempts < 5)
    {
      delay(500);
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      server.send(200, "text/plain", "Connected");
      resetFunc();
    }
    else
      server.send(200, "text/plain", "Unable to connect");
  }
}