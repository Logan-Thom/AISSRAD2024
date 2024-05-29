/*
  * Accelerometer functionality for Strath AIS Spaceport America Cup 2024 FC
  * Current Functionality: 6 axis readings, magnitude of acceleration, magnitude of rotation
  * Needed Functionality: optional (because difficult processing wise) calibration
  * Created For: Strath AIS
  * Created By: Logan Thom
  * Created On: 26/12/23
  * Last Updated: 12/02/24
  * Updated By: Logan Thom
*/

#ifndef Accelerometer_h
#define Accelerometer_h


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Accelerometer : public Adafruit_MPU6050
{
    public:
        Accelerometer();
        sensors_event_t a;
        sensors_event_t g;
        sensors_event_t temp;
        float XAcc;
        float YAcc;
        float ZAcc;
        float XSpin;
        float YSpin;
        float ZSpin;
        float XAOffset;
        float YAOffset;
        float ZAOffset;
        float XSOffset;
        float YSOffset;
        float ZSOffset;
        void UpdateSensorData();
        void Calibrate();
        float GetAccelerationMagnitude();
        float GetRotationMagnitude();

    private:
};




#endif
