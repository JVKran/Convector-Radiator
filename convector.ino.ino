#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "convectionRadiator.hpp"
#include "mqttClient.hpp"
#include "array.hpp"
#include "button.hpp"

array<const char*, 2> topics = {"/woonkamer/radiator", "/woonkamer/radiator/brightness"};
WiFiClient espClient;
mqttClient client("KraanBast2.4", "Snip238!", "192.168.178.74", topics, espClient);

convectionRadiator convector(D3, "/woonkamer/radiator/state", "/woonkamer/radiator/availability", client);
button fansEnabledButton(D0);
// temperatureSensor tempSensor(A0, "/woonkamer/radiator/temperatuur", client);

void callback(char* topic, byte* payload, unsigned int length) {
    for (int i = 0; i < length; i++) {
        client.message.concat((char)payload[i]);
    }
    for (int i = 0; i < client.amountOfListeners; i++){
        client.listeners[i]->messageReceived(client.message, topic);
    }
    client.sendMessage("Test", "Called back!");
    client.message = "";  
}

void setup() {
    client.addListener(convector);          //Has to be added because availability has got to be updated
    client.setupWifi();
    client.setupConnections();
    fansEnabledButton.addListener(convector);
    pinMode(D5, INPUT);
    pinMode(D1, OUTPUT);
    digitalWrite(D1, HIGH);
    digitalWrite(D1, LOW);
}

void loop() {
    client.checkForMessages();
    if(millis() > fansEnabledButton.lastCheck + 1000){
        fansEnabledButton.lastCheck = millis();
        fansEnabledButton.update();
    }
    // if (millis() > lastUpdate + 30000){
    //     tempSensor.publishTemperature();
    //     lastUpdate = millis();
    // }
}
