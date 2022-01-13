#ifndef DEVICE_H
#define DEVICE_H

#include "Arduino.h"
#include <vector>
#include <map>
#include <string>
#include <EEPROM.h>

// flag is EEPROM data exits
const byte EEPROM_DATA = 'k';

class Device
{
public:
  byte UUID[16];
  std::string Name;
  bool On;
  int State;
  int FeatureFlags;
  std::map<std::string, bool> BoolValues;
  std::map<std::string, int> IntValues;
  std::map<std::string, double> FloatValues;
  std::map<std::string, std::string> StringValues;
  
public:
    Device();

    std::string ToString();
    void Parse(std::string json);

private:
    bool isInt(std::string str);
    bool isFloat(std::string str);

    void trim(std::string& str);

    static std::vector<std::string> split (std::string s, std::string delimiter);
};
#endif
