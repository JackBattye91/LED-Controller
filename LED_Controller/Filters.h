#ifndef FILTER_H
#define FILTER_H
#include <FastLED.h>

class Filter
{
public:
  virtual void Update()
  {
    updated = true;
  }
   
  virtual CRGB* Apply(CRGB* leds)
  {
    
  }
protected:
  bool updateRequired()
  {
    if (lastUpdate + delayTimer < millis())
    {
      lastUpdate = millis();
      return true;
    }
  }

  bool updated;
  unsigned long lastUpdate;
  unsigned long delayTimer;
};

class Cycle : public Filter
{
public:
  enum DIRECTION
  {
    FOREWARD = 0,
    BACKWARD = 1
  };

  Cycle(int updateMillis = 1000, DIRECTION direction = FOREWARD)
  {
    dir = direction;
    delayTimer = updateMillis;
    lastUpdate = 0;
    offset = 0;
  }

  void Update()
  {
    if (updateRequired())
    {
      offset++;
      lastUpdate = millis();
      updated = true;
    }
    else
      updated = false;
  }
  
  CRGB* Apply(CRGB* leds)
  {
    if (updated)
    {
      int numLEDS = sizeof(leds) / sizeof(CRGB);
      CRGB* newLEDs = new CRGB[numLEDS];
  
      if (dir == FOREWARD)
      {
        for (int l = 0; l < numLEDS; l++)
          newLEDs[l] = leds[(l + offset) % numLEDS];
      }
      else
      {
        for (int l = 0; l < numLEDS; l++)
          newLEDs[(l + offset) % numLEDS] = leds[l];
      }
      
      return newLEDs;
    }
    
    return leds;
  }

private:
  DIRECTION dir;
  int offset;
};

class Twinkle : public Filter
{
public:
  Twinkle(int updateMillis = 1000)
  {
    odds = false;
    delayTimer = updateMillis;
  }

  virtual void Update()
  {
    if (updateRequired())
    {
      odds = !odds;
      updated = true;
    }
    else
      updated = false;
  }

  virtual CRGB* Apply(CRGB*& leds)
  {
    if (updated)
    {
      int numLEDS = sizeof(leds) / sizeof(CRGB);
      CRGB* newLEDs = new CRGB[numLEDS];
      
      for(int l = 0 ; l < numLEDS; l++)
      {
        if (odds)
        {
          if (l % 2 == 1)
            newLEDs[l] = leds[l];
          else
            newLEDs[l] = CRGB(0,0,0);
        }
        else
        {
          if (l % 2 == 0)
            newLEDs[l] = leds[l];
          else
            newLEDs[l] = CRGB(0,0,0);
        }
      }
      
      return newLEDs;
    }
    
    return leds;
  }

private:
  bool odds;
  int delayTimer;
  unsigned long lastUpdate;
};

class Blink : public Filter
{
public:
  Blink(int updateMillis = 1000)
  {
    isOn = false;
    delayTimer = updateMillis;
  }

  virtual void Update()
  {
    if (updateRequired())
    {
      isOn = !odds;
      updated = true;
    }
    else
      updated = false;
  }

  virtual CRGB* Apply(CRGB*& leds)
  {
    if (updated)
    {
      int numLEDS = sizeof(leds) / sizeof(CRGB);
      CRGB* newLEDs = new CRGB[numLEDS];
      
      for(int l = 0 ; l < numLEDS; l++)
      {
        if (isOn)
          newLEDs[l] = leds[l];
        else
          newLEDs[l] = CRGB(0,0,0);
      }
      
      return newLEDs;
    }
    
    return leds;
  }

private:
  bool odds;
  int delayTimer;
  unsigned long lastUpdate;
};
#endif
