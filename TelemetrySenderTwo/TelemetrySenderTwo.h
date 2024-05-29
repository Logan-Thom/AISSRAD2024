#ifndef TelemetrySenderTwo_h
#define TelemetrySenderTwo_h

#include "Arduino.h"
#include "TelemetryDataTwo.h" // Data structure 

class TelemetrySenderTwo {
public:
    TelemetrySenderTwo();
    void begin(long baudRate = 115200);
    void sendTelemetry(const TelemetryDataTwo& data);
private:
    void configureLoRaModule();
};

#endif
