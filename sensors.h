struct Sensor {
    int pin;
    float value;
    float threshold;
    char name[15];
};

struct Sensor sensors[] = {
    {A0, -999.0, 0.5, "temperature"},
    {A0, -999.0, 1.0, "hummidity"},
};

const int sensorCount = sizeof(sensors) / sizeof(Sensor);

// Buffer
bool bufferValid = false;
const int bufferSize = 60;
float buffer[sensorCount][bufferSize];
int bufferIndex = 0;



void printBuffer() {
  Serial.print("Buffer ");
  Serial.print(": ");
  for (int i = 0; i < bufferSize; i++) {
    Serial.print(buffer[0][i]);
    Serial.print(" ");
  }
Serial.print("    ");
  for (int i = 0; i < bufferSize; i++) {
    Serial.print(buffer[1][i]);
    Serial.print(" ");
  }
  Serial.println();
}

float getAvg(float values[]) {
  float sum = 0.0;

  for (int i = 0; i < bufferSize; i++) {
    sum += values[i];
  }

  return sum / bufferSize;
}


void readSensors(void (* callback)(char* topic, char *payload))
{
    // TODO senzory přesahující treshold vrátit v poli
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

                char topic[100];
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

    printBuffer();
    bufferIndex++;
}


