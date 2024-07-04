#include "mqtt.h";
#include "sensors.h";


void setup() {
    Serial.begin(115200);
    initMQTT();
}

void loop() {
    reconnectMQTT();
    readSensors(publishCallback);
    delay(1000);
}