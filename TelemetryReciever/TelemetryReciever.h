#ifndef TelemetryReciever_h
#define TelemetryReciever_h

#include "Arduino.h"

class TelemetryReciever {
public:
    TelemetryReciever();
    void begin(long baudRate = 115200);
    String receiveData();
private:
    void configureLoRaModule();
};

#endif
