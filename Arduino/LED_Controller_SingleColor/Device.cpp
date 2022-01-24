#include "Device.h"

Device::Device()
{
    randomSeed(analogRead(0));
    
    // if EEPROM data exists
    if (EEPROM[100] == EEPROM_DATA)
    {
        for(int b = 0; b < 16; b++)
            UUID[b] = EEPROM[b + 101];
        
        EEPROM.get(EEPROM_ON, On);
        EEPROM.get(EEPROM_STATE, State); // 4 bytes for state
        EEPROM.get(EEPROM_FEATURES, FeatureFlags); // 4 bytes for features
    }
    // generate UUID and save to EEPROM
    else
    {
        for(int b = 0; b < 16; b++)
            EEPROM[b + 1] = UUID[b] = random(128);
    
        On = false;
        State = 0;
        FeatureFlags = 0;

        EEPROM.put(EEPROM_ON, On);
        EEPROM.put(EEPROM_STATE, State);
        EEPROM.put(EEPROM_FEATURES, FeatureFlags);

        EEPROM[0] = EEPROM_DATA;
    }

    // setup defaults
    BoolValues = std::map<std::string, bool>();
    IntValues = std::map<std::string, int>();
    FloatValues = std::map<std::string, double>();
    StringValues = std::map<std::string, std::string>();
}

std::string Device::ToString()
{
    std::string json = "{";

    // add UUID
    json += "\t\"uuid\" : '";
    for(int b = 0; b < 16; b++)
        json += UUID[b];
    json += "',\n";

    // Add Name
    json += "\t\"name\" : '" + Name + "',\n";

    // Add On
    json += "\t\"on\" : ";
    if (On)
        json += "true,\n";
    else
        json += "false,\n";

    // Add State
    json += "\t\"state\" : " + std::to_string(State) + ",\n";

    // Add Features                                     
    json += "\t\"features\" : " + std::to_string(FeatureFlags) + ",\n";

    // Add Values
    json += "\t\"values\" : [ ";
    for (std::map<std::string, bool>::iterator it = BoolValues.begin(); it != BoolValues.end(); ++it)
    {
        std::string strBoolValue = it->second ? "true" : "false";
        json += "\t\t\"{ " + it->first + "\" : " + strBoolValue + " },\n";
    }
    for (std::map<std::string, int>::iterator it = IntValues.begin(); it != IntValues.end(); ++it)
        json += "\t\t\"{ " + it->first + "\" : " + std::to_string(it->second) + " },\n";
        
    for (std::map<std::string, double>::iterator it = FloatValues.begin(); it != FloatValues.end(); ++it)
        json += "\t\t\"{ " + it->first + "\" : " + std::to_string(it->second) + " },\n";
        
    for (std::map<std::string, std::string>::iterator it = StringValues.begin(); it != StringValues.end(); ++it)
        json += "\t\t\"{ " + it->first + "\" : \"" + it->second + "\" },\n";
        
    json += "\t],\n";

    json += "}";
    return json;
}
void Device::Parse(std::string json)
{
    size_t currPos = -1;

    if ((currPos = json.find("\"name\"")) > -1)
    {
        currPos += 6;
        int startPos = json.find("\"") + 1;
        int finishPos = json.find("\"", startPos);

        Name = json.substr(startPos, finishPos);

        Serial.print("Name: ");
        Serial.println(Name);
    }

    if ((currPos = json.find("\"on\"")) > -1)
    {
        currPos += 4;
        int startPos = json.find(":", currPos) + 1;
        int finishPos = json.find(",", startPos);

        if (json.substr(startPos, finishPos).find("true") != -1)
            On = true;
        else
            On = false;

        EEPROM.put(EEPROM_ON, On);

        Serial.print("On: ");
        Serial.println(On);
    }

    if ((currPos = json.find("\"state\"")) > -1)
    {
        currPos += 7;
        int startPos = json.find(":", currPos) + 1;
        int finishPos = json.find(",", startPos);

        State = std::stoi(json.substr(startPos, finishPos));

        EEPROM.put(EEPROM_STATE, State);

        Serial.print("State: ");
        Serial.println(State);
    }

    if ((currPos = json.find("\"values\"")) > -1)
    {
        currPos += 8;
        int startPos = json.find("[", currPos) + 1;
        int finishPos = json.find("]", startPos);

        std::vector<std::string> valueList = split(json.substr(startPos, finishPos), ",");

        for(int v = 0; v < valueList.size(); v++)
        {
            std::vector<std::string> valuePair = split(valueList[v], ":");

            if (valuePair.size() != 2)
                continue;

            trim(valuePair[0]);
            trim(valuePair[1]);

            while(valuePair[0].find('"') > -1)
                valuePair[0].erase(valuePair[0].find('"'), 1);

            if (valuePair[1].find('"') > -1)
            {
                while(valuePair[1].find('"') > -1)
                    valuePair[1].erase(valuePair[1].find('"'), 1);

                StringValues[valuePair[0]] = valuePair[1];
            }
            else if (isFloat(valuePair[1]))
            {
                FloatValues[valuePair[0]] = std::stod(valuePair[1]);
            }
            else if (isInt(valuePair[1]))
            {
                IntValues[valuePair[0]] = std::stoi(valuePair[1]);
            }
            else if (valuePair[1].find("true") > -1 || valuePair[1].find("false") > -1)
            {
                if (valuePair[1].find("true") > -1)
                    BoolValues[valuePair[0]] = true;
                else
                    BoolValues[valuePair[0]] = false;
            }

            Serial.print("Value: ");
            Serial.print(valuePair[0]);
            Serial.print(" = ");
            Serial.println(valuePair[1]);
        }
    }
}
bool Device::isInt(std::string str)
{
    for(int c = 0; c < str.length(); c++)
    {
        if (str[c] < '0' && str[c] > '9')
            return false;
    }
    return true;
}
bool Device::isFloat(std::string str)
{
    for(int c = 0; c < str.length(); c++)
    {
        if (str[c] < '0' && str[c] > '9' && str[c] != '.')
            return false;
    }
    return true;
}

void Device::trim(std::string& str)
{
    while(str[0] == ' ')
        str.erase(0, 1);
    
    while(str[str.length() - 1] == ' ')
        str.erase(str.length() - 1, 1);
}

std::vector<std::string> Device::split (std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}
