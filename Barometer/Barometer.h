/*
  * Header file for Barometer functionality of Strath AIS Spaceport America Cup 2024 FC
  * Current Functionality: Calibration, Altitude reading, Pressure reading, Descent detection
  * Needed Functionality: none
  * Created For: Strath AIS
  * Created By: Logan Thom
  * Created On: 26/11/23
  * Last Updated: 12/02/24
  * Updated By: Logan Thom
*/

#ifndef Barometer_h
#define Barometer_h

#include "Arduino.h"
#include "Adafruit_BMP3XX.h"
#include <Wire.h>
#include <math.h>

class Barometer : public Adafruit_BMP3XX
{
  public:
    Barometer();
    void Speak();
    void InitialiseSensor();
    void Calibrate();
    float GetCurrentHeight();
    float GetCurrentPressure();
    float GetCurrentTemperature();
    bool IsDescending();

    float zeroHeight;
    float currentPressure;
    float currentHeight;
    float currentTemp;
    float highestAltitude;
    int descentCount;               //counts how long rocket has been descending so as to not deploy parachute at incorrect altitude

  private:
    float _SeaLevelPressure = 1013.25;
};


#endif

