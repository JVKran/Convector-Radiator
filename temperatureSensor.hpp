#ifndef __TEMPSENSOR_HPP
#define __TEMPSENSOR_HPP

#include "mqttClient.hpp"

class temperatureSensor{
	private:
		const unsigned int tempPin;
		const char* topic;
		mqttClient & client;

		float getTemperature(){
			return float(((analogRead(tempPin)/1024.0)*3300) / 10);
		}

	public:
		temperatureSensor(const unsigned int tempPin, const char* topic, mqttClient & client): 
			tempPin(tempPin),
			topic(topic),
			client(client) 
		{}

		void publishTemperature(){
			client.sendMessage(topic, String(getTemperature()).c_str());
		}
};

#endif //__TEMPSENSOR_HPP