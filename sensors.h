/** Struktura pro data každého senzoru */
struct Sensor {
    int pin;
    float value;
    float threshold;
    char name[15];
};

/** Struktura s daty všech senzorů */
struct Sensor sensors[] = {
    {A0, -9999.0, 0.5, "temperature"},
    {A1, -9999.0, 1.0, "hummidity"},
};

/** Počet senzorů */
const int sensorCount = sizeof(sensors) / sizeof(Sensor);

/** Velikost bufferu */
const int bufferSize = 60;

/** Zda je buffer naplněn a jeho průměr je tedy validní hodnotou */
bool bufferValid = false;

/** Aktuální pozice v bufferu */
int bufferIndex = 0;

/** Buffer pro hodnoty všech senzorů */
float buffer[sensorCount][bufferSize];


/** Vrátí průměr hodnot v předávaném poli */
float getAvg(float values[]) {
  float sum = 0.0;
  for (int i = 0; i < bufferSize; i++) {
    sum += values[i];
  }
  return sum / bufferSize;
}

/** Přečte data ze všech senzorů, vypočítá jejich průměr a pokud je rozdíl od předchozí zaslané hodnoty větší nebo roven prahu (threshold), odešle hodnotu na přes MQTT */
void readSensors(void (* callback)(char* topic, char *payload))
{
    if (bufferIndex == bufferSize) {
        bufferIndex = 0;
        bufferValid = true;
    }

    for(int sensorIndex = 0; sensorIndex < sensorCount; sensorIndex++) {
        int pin = sensors[sensorIndex].pin;
        buffer[sensorIndex][bufferIndex] = ((analogRead(pin)/1024.0) * 5.0 - 0.5) * 100;
        if(bufferValid) {
            float currAvg = getAvg(buffer[sensorIndex]);

            if(abs(sensors[sensorIndex].value - currAvg) >= sensors[sensorIndex].threshold) {
                sensors[sensorIndex].value = currAvg;

                char topic[40];
                memset(topic, 0, sizeof(topic));
                strcat(topic, "greenhouse/sensors/");
                strcat(topic, sensors[sensorIndex].name);

                char convertBuffer[20];
                dtostrf(currAvg, 6, 2, convertBuffer);
                Serial.println(topic);
                callback(topic, convertBuffer);
            }
        }
    }
    bufferIndex++;
}


