#include "TelemetryReciever.h"

TelemetryReciever::TelemetryReciever() {
    // Constructor doesnt do anything
}

void TelemetryReciever::begin(long baudRate) {
    Serial1.begin(baudRate);
    configureLoRaModule();
}

String TelemetryReciever::recieveData() {
    if (Serial1.available()) {
        String recievedData = Serial1.readStringUntil('\n');
		Serial.println("Recieved data: " + recievedData);
        return recievedData;
    }
    return ""; // Return empty string if no data is available
}

void TelemetryReciever::configureLoRaModule() {
    // AT commands to configure the LoRa module
	// Set ADDRESS
    Serial1.println("AT+ADDRESS=3");  // Set address same on both modules

    // Set NETWORKID
    Serial1.println("AT+NETWORKID=5");  // Set network ID same on both modules

    // Set BAND
    Serial1.println("AT+BAND=433E6");  // 433 MHz check??/

    // Set PARAMETER
    Serial1.println("AT+PARAMETER=12,4,1,7");  // Parameter settings for over 3km
    Serial.println("LoRa module configured");
}
