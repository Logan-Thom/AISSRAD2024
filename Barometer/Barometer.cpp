#include "Arduino.h"
//#include "Adafruit_BMP3XX.h"
#include "Barometer.h"


//constructor function, doesn't actually need to do anything 
Barometer::Barometer()
{
  //Serial.println("Initialised Barometer Class");
}


//test function to see if class has been created properly
void Barometer::Speak()
{
  Serial.println("Hello");
}


void Barometer::Calibrate()
{
  //loop through to get an average of the '0' reading and set value to that
  float total;
  
  for (int ii=0;ii<20;ii++)
  {
    Barometer::readAltitude(_SeaLevelPressure);
  }
  
  for (int i=0;i<50;i++)
  {
    Serial.println("Calibrating Barometer");
    total += Barometer::readAltitude(_SeaLevelPressure);
    delay(20);
  }
  zeroHeight = total/50;
  highestAltitude = zeroHeight - 5;                   //this ensures the rocket will not immediately go into parachute deployment.
}

//Get pressure reading, input void, returns float
float Barometer::GetCurrentPressure()
{
  currentPressure = Barometer::readPressure();
  return currentPressure;
}

//Setup component with apropriate settings and sampling rates
void Barometer::InitialiseSensor()
{
  Serial.println("BMP warming up");
  float deadValue = 0.0;
  for (int i=0;i<10;i++)
  {
    deadValue = Barometer::readAltitude(_SeaLevelPressure);
    Serial.println(i);
    delay(500);
  }
}


//Get height reading, input void, returns float
float Barometer::GetCurrentHeight()
{
  currentHeight = Barometer::readAltitude(_SeaLevelPressure);
  if (currentHeight > highestAltitude)
  {

    Barometer::highestAltitude = currentHeight;
  }
  
  return currentHeight - zeroHeight;
}


//Get temp reading, input void, returns float
float Barometer::GetCurrentTemperature()
{
  currentTemp = Barometer::readTemperature();
  return currentTemp;
}


//check if rocket is falling, will be used to deploy drogue in combination with some extra logic
bool Barometer::IsDescending()
{
  if(currentHeight > highestAltitude)
  {
    descentCount = 0;
    highestAltitude = currentHeight;
    return false;
  }
  else if (currentHeight<(highestAltitude - 10.00))
  {    //alter value of constant where appropriate after testing, change to 500 - 1000 ms delay after apogee
    descentCount++;
    if (descentCount == 50)
    {
      return true;
    }
    return false;
  }
  else
  {
    descentCount = 0;   //very important to reset here or else false deployments will be common as descent count will increase forever on static-ish flights.
    return false;
  }
}
