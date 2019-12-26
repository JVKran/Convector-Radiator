#include "mqttClient.hpp"

void callback(char* topic, byte* payload, unsigned int length);

mqttClient::mqttClient(char* ssid, char* password, char* mqttServer, char* topic, WiFiClient & espClient):
	ssid(ssid),
	password(password),
	mqttServer(mqttServer),
	topic(topic),
	client(espClient)
{
	WiFi.mode(WIFI_STA);
	setupWifi();
    client.setServer(mqttServer, 1883);
    client.setCallback(callback);
    client.subscribe(topic);
}
