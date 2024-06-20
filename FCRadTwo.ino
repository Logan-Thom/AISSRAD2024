/*
  * SRAD FLight Computer for Strath AIS Spaceport America Cup 2024
  * Current Functionality: Working Barometer, Accelerometer, flight control logic, Radio, SD Card, Ignition Circuts
  * Needed Functionality: None :D
  * Created For: Strath AIS
  * Created By: Logan
  * Created On: 16/12/23
  * Last Updated: 10/06/24
  * Updated By: Logan, Jamie, Finlay, Alfredo
*/

#include <Barometer.h>
#include <Accelerometer.h>
#include <TelemetryDataTwo.h>
#include <TelemetrySenderTwo.h>
#include <Wire.h>
#include <DataLoggerSD.h>


//declare timing variables/constants
unsigned long startMillis;
unsigned long currentMillis;
unsigned long time_since_motor_burn;
unsigned long time_since_motor_burnout;
unsigned long time_main_deployed;
unsigned long time_drogue_deployed;

bool flight_state_active = false;
int incomingByte;

Barometer bmp;
Accelerometer mpu;
TelemetrySenderTwo telemetrySender;
TelemetryDataTwo data;

const int chipSelect = 32;
DataLoggerSD dlsd;

//start a flight status check for safety reasons and logic control
enum status { WeeBoyIdle,
              PoweredFlight,
              Coasting,
              Drogue,
              Main,
              Landed };
status flight_status;

//initialise flight data variables as empty length 0 globals
float acc;
float alt;
float pressure;
float rotational_velocity;
float maxAlt;

//important output pins for parachutes and safety
const int drogue_deploy_pin = 26;
const int main_deploy_pin = 24;
const int buzzer = 11;  //needs to be a PWM capable pin
const int gpsRead = 2;
const int ignRead = 38;
const int fakeGnd = 8;
bool GpsArmFlag = false;
bool IgnArmFlag = false;
bool garmed = false;
bool iarmed = false;

/*
String statusToString(status flightStatus) {
  switch (flightStatus) {
    case WeeBoyIdle: return "WeeBoyIdle";
    case PoweredFlight: return "PoweredFlight";
    case Coasting: return "Coasting";
    case Drogue: return "Drogue";
    case Main: return "Main";
    case Landed: return "Landed";
    default: return "Unknown";
  }
}
*/

void SoundBuzzer(unsigned long timer){
  Serial.println("buzzing");
  unsigned long delaytime = millis();
  while ((delaytime - timer) < 6000){
    digitalWrite(buzzer, HIGH);
    delayMicroseconds(500);
    digitalWrite(buzzer, LOW);
    delayMicroseconds(500);
    delaytime = millis();
  }
}



void setup() {

  //some pins initially set to input to prevent misfire of ignition circuits on program startup.
  pinMode(drogue_deploy_pin, INPUT_PULLUP);
  pinMode(main_deploy_pin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(gpsRead, INPUT);
  pinMode(ignRead, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);


  Serial.begin(115200);
  Serial3.begin(115200);


  Serial.println("Program Start");
  //dlsd.speak();




  //begin program timing
  startMillis = millis();

  //SoundBuzzer(startMillis);

  //=======================================================
  //Barometer Setup, looks awful but sadly its necessary
  //for it to actually work properly, dont worry too much
  //about how it works exactly.
  //Starts I2C on the chip, sets sample rates
  //=======================================================

  while (!Serial)
    ;

  if (!bmp.begin_I2C()) {  // hardware I2C mode, can pass in address & alt Wire
                           //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
                           //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring");
    while (!bmp.begin_I2C()){
      delay(200);
    }
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_16X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);  //recommended setting for drop detection, makes testing slightly unreliable as is not suited to static environments with people moving around it
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);


  bmp.Calibrate();

  //=======================================================
  //Accelerometer setup, mainly implementing ranges of
  //recorded data and constraints
  //=======================================================
  if (!mpu.begin()) {
    Serial.println("Could not find a valid mpu sensor, check wiring");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU_6050 located");
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu.Calibrate();
  delay(100);


  //=======================================================
  //SD card setup
  //=======================================================

  dlsd.Initialise();

  dlsd.removeOld("offsets.txt");
  dlsd.removeOld("datalog.txt");

  float offsets[] = { mpu.XAOffset, mpu.YAOffset, mpu.ZAOffset, mpu.XSOffset, mpu.YSOffset, mpu.ZSOffset, bmp.zeroHeight };  //currently only using one offset
  dlsd.newOffsetlog("offsets.txt", 7, "XAOffset,YAOffset,ZAOffset,XSOffset,YSOffset,ZSOffset,zeroHeight", offsets);
  dlsd.newDatalog("datalog.txt", "acc,spin,alt,pressure");


  //=======================================================
  //Radio and telemetry setup
  //=======================================================


  //set flight status
  flight_status = WeeBoyIdle;  //thanks, Jack.

  //ignition circuits setup
  pinMode(drogue_deploy_pin, OUTPUT);
  pinMode(main_deploy_pin, OUTPUT);

  Serial.println("Setup complete");
  Serial3.println("AT+SEND=3,9,CONNECTED\r\n");
  Serial3.flush();

  


  flight_state_active = false;

//buzzing
  currentMillis=millis();
  SoundBuzzer(currentMillis);

  Serial.print("Gps and Ign: ");
  Serial.print(GpsArmFlag);
  Serial.println(IgnArmFlag);

/*
  while (GpsArmFlag == false || IgnArmFlag == false){
    currentMillis = millis();
    GpsArmFlag = digitalRead(gpsRead);
    IgnArmFlag = digitalRead(ignRead);
    if(IgnArmFlag == true && iarmed == false){
      iarmed = true;
      SoundBuzzer(currentMillis);
    }
    if(GpsArmFlag == true && garmed == false){
      garmed = true;
      SoundBuzzer(currentMillis);
    }
  }
*/

  while (IgnArmFlag == false){
    currentMillis = millis();
    IgnArmFlag = digitalRead(ignRead);
    if(IgnArmFlag == true && iarmed == false){
      iarmed = true;
      SoundBuzzer(currentMillis);
    }
  }

}

void loop() {


  //update timer variable
  currentMillis = millis();


  //update, store and transmit data
  mpu.UpdateSensorData();
  acc = mpu.GetAccelerationMagnitude();
  pressure = bmp.GetCurrentPressure();
  alt = bmp.GetCurrentHeight();
  maxAlt = bmp.highestAltitude - bmp.zeroHeight;
  rotational_velocity = mpu.GetRotationMagnitude();



  //perform flight logic
  switch (flight_status) {
    case 0:
      if (acc > 90) {
        //Big Boy Launch

        flight_status = PoweredFlight;
        time_since_motor_burn = millis();
      }
      break;

    case 1:
      if (acc < 30 && time_since_motor_burn > 2000) {
        flight_status = Coasting;
        time_since_motor_burnout = millis();
      }
      break;

    case 2:
      if (bmp.IsDescending() == true && time_since_motor_burnout > 2000) {
        if (flight_state_active) {
          digitalWrite(drogue_deploy_pin, HIGH);
          Serial3.println("AT+SEND=3,14,DROGUEDEPLOYED\r\n");
          time_drogue_deployed = millis();
        }

        flight_status = Drogue;
      }
      break;

    case 3:
      if (alt < 1450 && (currentMillis - time_drogue_deployed) > 3000) {
        if (flight_state_active) {
          digitalWrite(main_deploy_pin, HIGH);
          Serial3.println("AT+SEND=3,12,MAINDEPLOYED\r\n");
          time_main_deployed = millis();
        }

        flight_status = Main;
      }
      break;

    case 4:
      if (currentMillis - time_main_deployed > 240000 && flight_state_active == true) {
        flight_status = Landed;
      }
      break;

    case 5:
      //(hopefully) flight successful
      //do final things to save data
      Serial3.println("Landed");
      digitalWrite(drogue_deploy_pin, LOW);  //just in case
      digitalWrite(main_deploy_pin, LOW);
      for (;;)
        ;
      break;
  }  //perform flight logic


  //data transfer and storage

  //telemetry data collection
  data.pressure = pressure;
  data.altitude = alt;
  data.maxAltitude = maxAlt;
  data.acceleration = acc;
  data.rotation = rotational_velocity;
  data.rocketState = flight_status;


  if (flight_state_active) {
    telemetrySender.sendTelemetry(data);
  }



  if (Serial3.available() > 0) {
    incomingByte = Serial3.read();
    if (incomingByte == 80) {
      telemetrySender.sendTelemetry(data);
      Serial.println("Telemetry request recieved");

    } else if (incomingByte == 83) {
      Serial.println("Telemetry Request Recieved");
      flight_state_active = true;
      flight_status = WeeBoyIdle;

      //important to prevent immediate firing of drogue on startup, likely wont happen.
      //bmp.highestAltitude = alt - bmp.zeroHeight;
      maxAlt = alt;
      bmp.highestAltitude = bmp.currentHeight;

    } else if (incomingByte == 66) {
      flight_status = WeeBoyIdle;
      //bmp.highestAltitude = alt;
      Serial.println("Telemetry Request Recieved");
      maxAlt = alt;
      bmp.highestAltitude = bmp.currentHeight;
    } 

    else{
      //
    }
  }


  //sd card data logging
  dlsd.newEntry(currentMillis);
  dlsd.appendData(acc);
  dlsd.appendData(mpu.XAcc);
  dlsd.appendData(mpu.YAcc);
  dlsd.appendData(mpu.ZAcc);
  dlsd.appendData(rotational_velocity);
  dlsd.appendData(mpu.XSpin);
  dlsd.appendData(mpu.YSpin);
  dlsd.appendData(mpu.ZSpin);
  dlsd.appendData(alt);
  dlsd.appendData(pressure);
  dlsd.appendData(flight_status);
  dlsd.saveEntry("datalog.txt");
}


