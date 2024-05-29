#include "Arduino.h"
#include "DataLoggerSD.h"


DataLoggerSD::DataLoggerSD()
{
  /*
  Serial.println("Initializing SD card...");
  // Check if card present and can be initialised
  pinMode(chipSelect, OUTPUT);    // Chip select pin must be set to OUTPUT mode
  if (!SD.begin(chipSelect)) {
    Serial.println("Failed: check if SD card present or module connected");
    return;                       // Halt program if not detected
  }
  Serial.println("Done");
  */
  
  //do nothing
}

void DataLoggerSD::speak(){
  Serial.println("SD test");
}

void DataLoggerSD::Initialise(){
  pinMode(_chipSelect, OUTPUT);
  if (!SD.begin(_chipSelect)) {
    Serial.println("SD Failed: check wiring");
    return;
  }
  Serial.println("SD Done");
}

void DataLoggerSD::removeOld(const char * filePath) {
  if (SD.exists(filePath)) {
    Serial.println("Attempting to remove old file");
    if (SD.remove(filePath)) {    // Successful file delete
      Serial.println("Done");
    }
    else {                        // Unsuccessful file delete
      Serial.println("Failed");
    }
  }
}

void DataLoggerSD::newOffsetlog(const char *filePath, int numOffsets, const char *offsetNames, float *offsetValues)
{   // Creates new offset log file
  File dataFile = SD.open(filePath, FILE_WRITE);
  if (dataFile) {
    Serial.println("Offsets file created");
    dataFile.println(offsetNames);
    Serial.println("Offsets header written");
    _dataline[0] = 0;                           // Reset dataline
    for (int i = 0; i <= numOffsets; i++) {     // Loop through offsets and add to file
      //dtostrf(offsetValues[i], 6, 3, _buffer);  // Convert float --> char array
      
      snprintf(_buffer, sizeof(_buffer), "%f", offsetValues[i]);
      
      strcat(_dataline, ",");                   // Append delimeter before data
      strcat(_dataline, _buffer);               // Add to data character array
    }
    strcat(_dataline, 0);                       // Terminate char array correctly
    dataFile.println(_dataline);                // Write offset value line to file
    dataFile.close();
    Serial.println("Offset values written");
  }
  else {
    Serial.println("Failed to open offsets log file");
  }
}

void DataLoggerSD::newDatalog(const char * filePath, const char * varNames)
{   // creates new datalog file
  File dataFile = SD.open(filePath, FILE_WRITE);
  if (dataFile) {
    Serial.println("Datalog file created");
    dataFile.println(varNames);
    dataFile.close();
    Serial.println("Datalog header written");
  }
  else {
    Serial.println("Failed to open datalog file");
  }
}

void DataLoggerSD::newEntry(long timestamp)
{
  _dataline[0] = 0;               // Reset dataline for new entry
  ltoa(timestamp, _buffer, 10);   // Convert long --> char array, add to buffer
  strcat(_dataline, _buffer);     // Add to data character array
}

void DataLoggerSD::appendData(float data)
{
  //dtostrf(data, 6, 3, _buffer);   // Convert float --> char array
  
  snprintf(_buffer, sizeof(_buffer), "%f", data);
  
  strcat(_dataline, ",");         // Append delimeter before data
  strcat(_dataline, _buffer);     // Add to data character array
}

void DataLoggerSD::saveEntry(const char * filePath)
{
  strcat(_dataline, 0);               // Terminate char array correctly
  File dataFile = SD.open(filePath, FILE_WRITE);
  if (_dataline) {                    // If file openned successfully
    dataFile.println(_dataline);      // Write data line
    dataFile.close();                 // Close file
    //Serial.println(_dataline);        // Print to the serial port too
  }
  else {
    Serial.println("Failed to open 'datalog.txt'");
  }
}
