#include "Arduino.h"
#include "Accelerometer.h"
#include <math.h>

Accelerometer::Accelerometer()
{
    //constructor doesn't do anything
}



void Accelerometer::UpdateSensorData()
{
    
    bool tempBool = Accelerometer::getEvent(&a, &g, &temp);
    Accelerometer::XAcc = a.acceleration.x;
    Accelerometer::YAcc = a.acceleration.y;
    Accelerometer::ZAcc = a.acceleration.z;
    Accelerometer::XSpin = g.gyro.x;
    Accelerometer::YSpin = g.gyro.y;
    Accelerometer::ZSpin = g.gyro.z;
}

void Accelerometer::Calibrate()
{
    float XATemp = 0;
    float YATemp = 0;
    float ZATemp = 0;
    float XSTemp = 0;
    float YSTemp = 0;
    float ZSTemp = 0; 
    for (int i=0;i<10;i++){
        Accelerometer::UpdateSensorData();
        XATemp += XAcc;
        Serial.print("AccelerationX: ");
        Serial.println(XAcc);
        Serial.println(XATemp);
        YATemp += YAcc;
        Serial.print("AccelerationY: ");
        Serial.println(YAcc);
        Serial.println(YATemp);
        ZATemp += ZAcc;
        Serial.print("AccelerationZ: ");
        Serial.println(ZAcc);
        Serial.println(ZATemp);
        XSTemp += XSpin;
        Serial.print("AccelerationX: ");
        Serial.println(XSpin);
        Serial.println(XSTemp);
        YSTemp += YSpin;
        Serial.print("AccelerationY: ");
        Serial.println(YSpin);
        Serial.println(YSTemp);
        ZSTemp += ZSpin;
        Serial.print("AccelerationZ: ");
        Serial.println(ZSpin);
        Serial.println(ZSTemp);
    }
    Accelerometer::XAOffset = XATemp / 10;
    Accelerometer::YAOffset = YATemp / 10;
    Accelerometer::ZAOffset = ZATemp / 10;
    Accelerometer::XSOffset = XSTemp / 10;
    Accelerometer::YSOffset = YSTemp / 10;
    Accelerometer::ZSOffset = ZSTemp / 10;
}

float Accelerometer::GetAccelerationMagnitude()
{
    double tempNumAcc = (Accelerometer::XAcc - Accelerometer::XAOffset)*(Accelerometer::XAcc - Accelerometer::XAOffset) + (Accelerometer::YAcc - Accelerometer::YAOffset)*(Accelerometer::YAcc - Accelerometer::YAOffset) + (Accelerometer::ZAcc - Accelerometer::ZAOffset)*(Accelerometer::ZAcc - Accelerometer::ZAOffset);
    return sqrtl(tempNumAcc);
}

float Accelerometer::GetRotationMagnitude()
{
    double  tempNum = (Accelerometer::XSpin - Accelerometer::XSOffset)*(Accelerometer::XSpin - Accelerometer::XSOffset) + (Accelerometer::YSpin - Accelerometer::YSOffset)*(Accelerometer::YSpin - Accelerometer::YSOffset) + (Accelerometer::ZSpin - Accelerometer::ZSOffset)*(Accelerometer::ZSpin - Accelerometer::ZSOffset);
    return sqrtl(tempNum);
}


