#ifndef DEVICE_H
#define DEVICE_H
#include <vector>
#include <map>
#include <string>

class Device
{
public:
    Device()
    {
        for(int b = 0; b < 36; b++)
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
        for (int f = 0; f < Features.count(); f++)          //      "feature_1",
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
            int startPos = json.indexOf("[", currPos) + 1;
            int finishPos = json.indexOf("]", startPos);

            Features = split(json.substr(startPos, finishPos), ",");
        }

        if ((currPos = json.indexOf("\"values\"")) > -1)
        {
            currPos += 8;
            int startPos = json.indexOf("[", currPos) + 1;
            int finishPos = json.indexOf("]", startPos);

            Features = split(json.substr(startPos, finishPos), ",");
        }
    }

    byte UUID[16];
    std:::string Name;
    bool On;
    int State;
    std::vector<std::string> Features;
    std::map<std::string, bool> boolValues;
    std::map<std::string, int> intValues;
    std::map<std::string, double> floatValues;
    std::map<std::string, std::string> strValues;

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

    static vector<string> split (string s, string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        string token;
        vector<string> res;

        while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }

        res.push_back (s.substr (pos_start));
        return res;
    }
};
#endif