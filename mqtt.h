#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

/** MAC adresa Arduina */
const byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

/** Název Arduina pro komunikaci s MQTT */
const char clientName[] = "arduino";

/** IP adresa MQTT serveru */
const IPAddress mqttServer(192, 168, 1, 103);

/** Port MQTT serveru */
const int mqttPort = 1883;

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

/** Callback pro příjem zpráv */
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

/** Callback pro odesílání zpráv */
void publishCallback(char* topic, char *payload)
{
    Serial.print(topic);
    Serial.println(payload);
    mqttClient.publish(topic, payload);
}

/** Inicializace ethernetu a spojení s MQTT serverem */
void initMQTT()
{
    // Inicializace ethernetu
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Chyba při získávání IP adresy.");
        while (true);
    }

    // Definice připojení k MQTT serveru
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(subscribeCallback);

    // Připojení k MQTT serveru
    if (mqttClient.connect(clientName)) {
        Serial.println("Pripojeno");

        // Přihlášení k odběru
        if (mqttClient.subscribe("greenhouse/windows")) {
            Serial.println("Přihlášeno");
        }
        else {
            Serial.println("Chyba při přihlašování");
        }
    }
    else {
        Serial.println("Odpojeno");
    }
}

/** Pokud nejsme připojeni k MQTT serveru tak se pokusíme připojit */
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
    mqttClient.loop(); // Zpracovani prichozich zprav
}