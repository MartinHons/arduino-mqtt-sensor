#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const IPAddress mqttServer(192, 168, 1, 103);
const int mqttPort = 1883;
const char clientName[] = "arduino";

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);



void subscribeCallback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Přijatá zpráva na téma: ");
    Serial.println(topic);

    Serial.print("Obsah zprávy: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void publishCallback(char* topic, char *payload)
{
    Serial.print(topic);
    Serial.println(payload);
    mqttClient.publish(topic, payload);
}



void initMQTT()
{
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Chyba při získávání IP adresy.");
        while (true);
    }

    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(subscribeCallback);

    // Připojení k MQTT serveru
    if (mqttClient.connect(clientName)) {
            Serial.println("Pripojeno");

        if (mqttClient.subscribe("greenhouse/windows")) {
            Serial.println("Přihlášeno k tématu 'pokus'.");
        } else {
            Serial.println("Chyba při přihlašování k tématu 'pokus'.");
        }
    } else {
        Serial.println("Odpojeno");
    }
}

void reconnectMQTT()
{
    if (!mqttClient.connected()) {
        if (mqttClient.connect(clientName)) {
            Serial.println("Pripojeno");
        }
        else {
            Serial.println("Odpojeno");
        }
    }
    mqttClient.loop(); // Zpracovani prichozich
}