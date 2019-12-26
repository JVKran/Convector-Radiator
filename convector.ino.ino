#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "convectionRadiator.hpp"
#include "temperatureSensor.hpp"
#include "mqttClient.hpp"

unsigned long lastUpdate;
unsigned long lastFanCheck;
unsigned long lastButtonCheck;

WiFiClient espClient;
mqttClient client("KraanBast2.4", "Snip238!","192.168.178.74", "/woonkamer/radiator", espClient);

convectionRadiator fans(D3, "/woonkamer/radiator/state", "/woonkamer/radiator/availability", client);
temperatureSensor tempSensor(A0, "/woonkamer/radiator/temperatuur", client);

void callback(char* topic, byte* payload, unsigned int length) {
    for (int i = 0; i < length; i++) {
        client.message.concat((char)payload[i]);
    }
    for (int i = 0; i < client.amountOfListeners; i++){
        client.listeners[i]->messageReceived(client.message);
    }
    client.message = "";  
}

void setup() {
    pinMode(D5, INPUT);
    pinMode(D1, OUTPUT);
    pinMode(D0, INPUT);
    digitalWrite(D1, HIGH);
    digitalWrite(D1, LOW);
}

void loop() {
    client.checkForMessages();
    if (millis() > lastUpdate + 30000){
        tempSensor.publishTemperature();
        lastUpdate = millis();
    }
}
