#ifndef DEVICE_H
#define DEVICE_H
#include <list>
#include <map>

class Device
{
public:
    Device()
    {
        for(int b = 0; b < 16; b++)
            UUID[b] = 0;
        Name =  String();
        On = false;
        State = 0;
        Features = std::list<String>();

        boolValues = std::map<String, bool>();
        intValues = std::map<String, int>();
        floatValues = std::map<String, double>();
        strValues = std::map<String, String>();
    }

    virtual String ToString()
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
        for (int f = 0; f < Features.Count(); f++)          //      "feature_1",
            json += "\t\t\"" + Features[f] + "\",\n";       //      "feature_2",
        json += "\t],\n";                                   //  ],

        // Add Values
        json += "\t\"values\" : [ ";
        for (std::map<String,String>::iterator it = boolValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : " + String(it->second) + " },\n";
        for (std::map<String,String>::iterator it = intValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : " + String(it->second) + " },\n";
        for (std::map<String,String>::iterator it = floatValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : " + String(it->second) + " },\n";
        for (std::map<String,String>::iterator it = strValues.begin(); it != Values.end(); ++it)
            json += "\t\t\"{ " it->first + "\" : \"" + it->second + "\" },\n";
        json += "\t],\n";

        json += "}";
        return json;
    }
    virtual void Parse(String json)
    {
        if (int uuidPos = json.indexOf("uuid") > -1)
        {
            int currPos = json.indexOf("{", uuid) + 1;
            for(int b = 0; b < 16; b++)
            {

            }
        }

        if (int namePos = json.indexOf("name") > -1)
        {
            int currPos = json.indexOf("\"", namePos) + 1;
            Name = json.substring(currPos, json.indexOf("\""), currPos));
        }

        if (int onPos = json.indexOf("on") > -1)
        {

        }

        if (int statePos = json.indexOf("state") > -1)
        {
            
        }

        if (int featurePos = json.indexOf("features") > -1)
        {

        }

        if (int valuePos = json.indexOf("values") > -1)
        {

        }
    }

    byte UUID[16];
    String Name;
    bool On;
    int State;
    std::list<String> Features;
    std::map<String, bool> boolValues;
    std::map<String, int> intValues;
    std::map<String, double> floatValues;
    std::map<String, String> strValues;

private:
    bool isInt(String str)
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

    int IntParse(String str)
    {
        int b = 0;

        for(int c = 0; c < str.length(); c++)
        {
            if (str[c] >= '0' && str[c] <= '9')
            {
                b *= 10;
                b += str[c] - '0';
            }
            else if (str[c] == '.')
                break;
        }

        return b;
    }
};
#endif