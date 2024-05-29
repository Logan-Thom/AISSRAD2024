#ifndef DataLoggerSD_h
#define DataLoggerSD_h

#include <SD.h>
#include <SPI.h>

class DataLoggerSD
{
  public:
    void speak();
    DataLoggerSD();
    void Initialise();
    void removeOld(const char * filePath);
    void newOffsetlog(const char *filePath, int numOffsets, const char *offsetNames, float *offsetValues);
    void newDatalog(const char * filePath, const char * varNames);
    void newEntry(long timestamp);
    void appendData(float data);
    void saveEntry(const char * filePath);
  private:
    char _buffer[11];           // NOTE: based on floats with 3 decimal digits and 6 whole digits + 2 chars for decimal point & end character
    char _dataline[144];        // NOTE: length is based on 12 measurements, each of 11 characters, + 2 chars for start/end characters
    int _chipSelect = 32;       //
};

#endif
