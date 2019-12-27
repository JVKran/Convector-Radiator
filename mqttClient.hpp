#ifndef __CONNECTIONS_HPP
#define __CONNECTIONS_HPP

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "array.hpp"

class messageListener {
    public:
        virtual void messageReceived(const String & receivedMessage, const char* topic = "") = 0;
};

void callback(char* topic, byte* payload, unsigned int length);

class mqttClient {
    private:
        char* ssid;
        char* password;
        char* mqttServer;
        array<const char*, 2> topics;
        PubSubClient client;

        #define MSG_BUFFER_SIZE  (50)
        char msg[MSG_BUFFER_SIZE];
        int value = 0;

    public:
        String message;
        messageListener * listeners[5] = {};
        uint8_t amountOfListeners = 0;

        mqttClient(char* ssid, char* password, char* mqttServer, array<const char*, 2> topics, WiFiClient & espClient):
            ssid(ssid),
            password(password),
            mqttServer(mqttServer),
            topics(topics),
            client(espClient)
        {
            WiFi.mode(WIFI_STA);
        }

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
                delay(500);
            }
        }

        void setupConnections(){
            client.setServer(mqttServer, 1883);
            client.setCallback(callback);
            for(const auto & topic : topics){
                client.subscribe(topic);
            }
            reconnect();                //Establish a connection by signing in with credentials.
            sendMessage("debug", "Started up convector");
        }

        void reconnect() {
            while (!client.connected()) {
                if (client.connect("ConvectorRadiator", "ConvectorClient", "Snip238!")) {
                    for(const auto & topic : topics){
                        client.subscribe(topic);
                    }
                    message = "CONNECTED";
                    for (int i = 0; i < amountOfListeners; i++){
                        listeners[i]->messageReceived(message);
                    }
                    message = "";
                    break;
                } else {
                    delay(1000);
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