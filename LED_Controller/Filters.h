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
   
  virtual void Apply(CRGB*& leds)
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
  }

  void Update()
  {
    if (updateRequired())
    {
      lastUpdate = millis();
      updated = true;
    }
    else
      updated = false;
  }
  
  void Apply(CRGB*& leds)
  {
    if (updated)
    {
      int numLEDS = sizeof(leds) / sizeof(CRGB);
  
      if (dir == FOREWARD)
      {
        for (int l = 0; l < numLEDS; l++)
          leds[l] = leds[(l + 1) % numLEDS];
      }
      else
      {
        for (int l = 0; l < numLEDS; l++)
          leds[(l + 1) % numLEDS] = leds[l];
      }
    }
  }

private:
  DIRECTION dir;
};

class Twinkle : public Filter
{
public:
  Twinkle(int updateMillis = 1000)
  {
    odds = false;
    delayTimer = updateMillis;
    originalLEDs = 0;
  }

  void Update()
  {
    if (updateRequired())
    {
      odds = !odds;
      updated = true;
    }
    else
      updated = false;
  }

  void Apply(CRGB*& leds)
  {
    if (originalLEDs = 0)
    {
      int numLEDs = sizeof(leds) / sizeof(CRGB);
      originalLEDs = new CRGB[numLEDs];

      for (int l = 0; l < numLEDs; l++)
        originalLEDs[l] = leds[l];
    }
    
    if (updated)
    {
      int numLEDs = sizeof(leds) / sizeof(CRGB);
      
      for(int l = 0 ; l < numLEDs; l++)
      {
        if (odds)
        {
          if (l % 2 == 1)
            leds[l] = originalLEDs[l];
          else
            leds[l] = CRGB(0,0,0);
        }
        else
        {
          if (l % 2 == 0)
            leds[l] = originalLEDs[l];
          else
            leds[l] = CRGB(0,0,0);
        }
      }
    }
  }

private:
  bool odds;
  CRGB* originalLEDs;
};

class Blink : public Filter
{
public:
  Blink(int updateMillis = 1000)
  {
    isOn = false;
    delayTimer = updateMillis;
    originalLEDs = 0;
  }

  void Update()
  {
    if (updateRequired())
    {
      isOn = !isOn;
      updated = true;
    }
    else
      updated = false;
  }

  void Apply(CRGB*& leds)
  {
    if (originalLEDs = 0)
    {
      int numLEDs = sizeof(leds) / sizeof(CRGB);
      originalLEDs = new CRGB[numLEDs];

      for (int l = 0; l < numLEDs; l++)
        originalLEDs[l] = leds[l];
    }
    
    if (updated)
    {
      int numLEDs = sizeof(leds) / sizeof(CRGB);

      for(int l = 0 ; l < numLEDs; l++)
      {
        if (isOn)
          leds[l] = originalLEDs[l];
        else
          leds[l] = CRGB(0,0,0);
      }
    }
  }

private:
  bool isOn;
  CRGB* originalLEDs;
};
#endif
