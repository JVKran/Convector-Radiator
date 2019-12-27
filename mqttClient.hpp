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
    public:
        messageListener * listeners[5] = {};
        uint8_t amountOfListeners = 0;

        mqttClient(char* ssid, char* password, char* mqttServer, array<const char*, 2> topics, WiFiClient & espClient);

        void addListener(messageListener & listener);
        void notifyListeners(const String & mesage, const char* topic);

        void setupWifi();
        void setupConnections();
        void reconnect();

        void checkForMessages();
        void sendMessage(const char* topic, const char* messageToSend);
};

#endif //__CONNECTIONS_HPP