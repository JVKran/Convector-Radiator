#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "convectionRadiator.hpp"
#include "mqttClient.hpp"
#include "array.hpp"
#include "button.hpp"

array<const char*, 2> topics = {"/woonkamer/radiator", "/woonkamer/radiator/brightness"};
WiFiClient espClient;
mqttClient client("SSID", "WPA", "Broker-IP", topics, espClient);

convectionRadiator convector(D3, "/woonkamer/radiator/state", "/woonkamer/radiator/availability", "/woonkamer/radiator/brightness/state", client);
button fansEnabledButton(D0);
// temperatureSensor tempSensor(A0, "/woonkamer/radiator/temperatuur", client);

void callback(char* topic, byte* payload, unsigned int length) {
    static String message;
    for (int i = 0; i < length; i++) {
        message.concat((char)payload[i]);
    }
    client.notifyListeners(message, topic);
    message = "";  
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
    if(millis() > fansEnabledButton.lastCheck + 250){
        fansEnabledButton.lastCheck = millis();
        fansEnabledButton.update();
    }
    // if (millis() > lastUpdate + 30000){
    //     tempSensor.publishTemperature();
    //     lastUpdate = millis();
    // }
}
