#ifndef __CONVECTOR_HPP
#define __CONVECTOR_HPP

#include "Arduino.h"
#include "mqttClient.hpp"
#include "button.hpp"

class convectionRadiator : public buttonListener, public messageListener {
	private:
		mqttClient & client;
		const char* stateTopic;
		const char* availabilityTopic;

		const unsigned int pwmPin;
		const int rpmPins[4] = {D5, D6, D7, D2};

		unsigned int lastKnownSpeed = 500;
		bool fansEnabled = true;
	public:
		convectionRadiator(const unsigned int pwmPin, const char* stateTopic, const char* availabilityTopic, mqttClient & client);

		bool setFanSpeed(const unsigned int fanSpeed);
		unsigned int getFanSpeed(const unsigned int pin);

		void turnOn();
		void turnOff();
		bool turnedOn();

		void updateAvailability(const bool stateAvailable);
		void checkObstruction();

		virtual void messageReceived(const String & receivedMessage, const char* topic = "");

		virtual void buttonPressed();
		virtual void buttonReleased();
};

#endif //__CONVECTOR_HPP