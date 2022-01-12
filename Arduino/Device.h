#ifndef DEVICE_H
#define DEVICE_H
#include <vector>
#include <map>
#include <string>
#include <EEPROM.h>

// flag is EEPROM data exits
const byte EEPROM_DATA 'k';

class Device
{
public:
    Device()
    {
        randomSeed(analogRead(0));
        
        // if EEPROM data exists
        if (EEPROM[0] == EEPROM_DATA)
        {
            for(int b = 0; b < 16; b++)
                UUID[b] = EEPROM[b + 1];
        }
        // generate UUID and save to EEPROM
        else
        {
            for(int b = 0; b < 16; b++)
                EEPROM[b + 1] = UUID[b] = random(128);
            
            EEPROM[0] = EEPROM_DATA;
        }

        // setup defaults
        Name =  String();
        On = false;
        State = 0;
        Features = std::list<String>();

        BoolValues = std::map<String, bool>();
        IntValues = std::map<String, int>();
        FloatValues = std::map<String, double>();
        StringValues = std::map<String, String>();
    }

    virtual std::string ToString()
    {
        String json = "{";
  
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
        json += "\t\"state\" : " + String(State) + ",\n";

        // Add Features                                     
        json += "\t\"features\" : [\n";                     // "Features" : [
        for (int f = 0; f < Features.count(); f++)          //      "feature_1",
            json += "\t\t\"" + Features[f] + "\",\n";       //      "feature_2",
        json += "\t],\n";                                   //  ],

        // Add Values
        json += "\t\"values\" : [ ";
        for (std::map<std::string, bool>::iterator it = BoolValues.begin(); it != Values.end(); ++it)
        {
            std::string strBoolValue = it->second ? "true" : "false";
            json += "\t\t\"{ " it->first + "\" : " + strBoolValue + " },\n";
        }
        for (std::map<std::string, int>::iterator it = IntValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : " + std::to_string(it->second) + " },\n";
        for (std::map<std::string, double>::iterator it = FloatValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : " + std::to_string(it->second) + " },\n";
        for (std::map<std::string, std::string>::iterator it = StringValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : \"" + it->second + "\" },\n";
        json += "\t],\n";

        json += "}";
        return json;
    }
    virtual void Parse(std::string json)
    {
        size_t currPos = -1;

        if ((currPos = json.indexOf("\"name\"")) > -1)
        {
            currPos += 6;
            int startPos = json.indexOf("\"") + 1;
            int finishPos = json.indexOf("\"", startPos);

            Name = json.substr(startPos, finishPos));
        }

        if ((currPos = json.indexOf("\"on\"")) > -1)
        {
            currPos += 4;
            int startPos = json.indexOf(":", currPos) + 1;
            int finishPos = json.indexOf(",", startPos);

            if (json.substr(startPos, finishPos).indexof("true") != -1)
                On = true;
            else
                On = false;
        }

        if ((currPos = json.indexOf("\"state\"")) > -1)
        {
            currPos += 7;
            int startPos = json.indexOf(":", currPos) + 1;
            int finishPos = json.indexOf(",", startPos);

            State = std::stoi(json.substr(startPos, finishPos));
        }

        if ((currPos = json.indexOf("\"features\"")) > -1)
        {
            currPos += 10;
            int startPos = json.indexOf(":", currPos) + 1;
            int finishPos = json.indexOf(",", startPos);

            FeatureFlags = std::stoi(json.substr(startPos, finishPos));
        }

        if ((currPos = json.indexOf("\"values\"")) > -1)
        {
            currPos += 8;
            int startPos = json.indexOf("[", currPos) + 1;
            int finishPos = json.indexOf("]", startPos);

            std::vector<std::string> valueList = split(json.substr(startPos, finishPos), ",");

            for(int v = 0; v < valueList.count(); v++)
            {
                std::string valuePair = split(valueList[v], ":");

                if (valuePair.count() != 2)
                    continue;

                trim(valuePair[0]);
                trim(valuePair[1]);

                while(valuePair[0].contains('"'))
                    valuePair[0].erase(valuePair[0].find('"'), 1)

                if (valuePair[1].contains('"'))
                {
                    while(valuePair[1].contains('"'))
                        valuePair[1].erase(valuePair[1].find('"'), 1)

                    StringValues[valuePair[0]] = valuePair[1];
                }
                else if (isFloat(valuePair[1]))
                {
                    FloatValues[valuePair[0]] = std::stod(valuePair[1]);
                }
                else if (isInt(valuePair[1])
                {
                    IntValues[valuePair[0]] = std::stoi(valuePair[1]);
                }
                else if (valuePair[1].contains("true") || valuePair[1].contains("false"))
                {
                    if (valuePair[1].contains("true"))
                        BoolValues[valuePair[0]] = true;
                    else
                        BoolValues[valuePair[0]] = false;
                }
            }
        }
    }

    byte UUID[16];
    std:::string Name;
    bool On;
    int State;
    int FeatureFlags;
    std::map<std::string, bool> BoolValues;
    std::map<std::string, int> IntValues;
    std::map<std::string, double> FloatValues;
    std::map<std::string, std::string> StringValues;

private:
    bool isInt(std::tring str)
    {
        for(int c = 0; c < str.length(); c++)
        {
            if (str[c] < '0' && str[c] > '9')
                return false;
        }
        return true;
    }
    bool isFloat(String str)
    {
        for(int c = 0; c < str.length(); c++)
        {
            if (str[c] < '0' && str[c] > '9' && str[c] != '.')
                return false;
        }
        return true;
    }

    void trim(std::string& str)
    {
        while(str[0] == ' ')
            str.erase(0, 1);
        
        while(str[str.length() - 1] == ' ')
            str.erase(str.length() - 1, 1);
    }

    static std::vector<string> split (std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        string token;
        std::vector<string> res;

        while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }

        res.push_back (s.substr (pos_start));
        return res;
    }
};
#endif