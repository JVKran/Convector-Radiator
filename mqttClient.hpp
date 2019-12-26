#ifndef __CONNECTIONS_HPP
#define __CONNECTIONS_HPP

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class messageListener {
    public:
        virtual void messageReceived(const String & receivedMessage) = 0;
};

class mqttClient {
    private:
        char* ssid;
        char* password;
        char* mqttServer;
        char* topic;
        PubSubClient client;

    public:
        String message;
        messageListener * listeners[5] = {};
        uint8_t amountOfListeners = 0;

        mqttClient(char* ssid, char* password, char* mqttServer, char* topic, WiFiClient & espClient);

        void addListener(messageListener & listener){
            if(amountOfListeners <= 4){
                listeners[amountOfListeners] = &listener;
                amountOfListeners++;
            }
        }

        void setupWifi() {
            delay(10);
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(200);
            }
            message = "CONNECTED";
            for (int i = 0; i < amountOfListeners; i++){
                listeners[i]->messageReceived(message);
            }
            message = "";
        }

        void reconnect() {
            message = "DISCONNECTED";
            for (int i = 0; i < amountOfListeners; i++){
                listeners[i]->messageReceived(message);
            }
            message = "";
            while (!client.connected()) {
                if (client.connect("ConvectorRadiator", "ConvectorClient", "Snip238!")) {
                    client.subscribe("/woonkamer/radiator");
                    message = "CONNECTED";
                    for (int i = 0; i < amountOfListeners; i++){
                        listeners[i]->messageReceived(message);
                    }
                    message = "";
                    break;
                } else {
                    delay(5000);
                }
            }
        }

        void checkForMessages(){
            if (!client.connected()) {
                reconnect();
            }
            client.loop();
        }

        void sendMessage(const char* topic, const char* messageToSend){
            client.publish(topic, messageToSend);
        }
};

#endif //__CONNECTIONS_HPP