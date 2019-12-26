#include "convectionRadiator.hpp"

convectionRadiator::convectionRadiator(const unsigned int pwmPin, const char* stateTopic, const char* availabilityTopic, mqttClient & client): 
	client(client),
	stateTopic(stateTopic),
	availabilityTopic(availabilityTopic),
	pwmPin(pwmPin)
{
	analogWriteFreq(25000);
	setFanSpeed(0);
}

bool convectionRadiator::setFanSpeed(const unsigned int fanSpeed){
	if(fanSpeed == 0){
		analogWrite(pwmPin, fanSpeed);
		client.sendMessage(stateTopic, "OFF");
		digitalWrite(D1, LOW);
	} else if(buttonPressed){
		analogWrite(pwmPin, fanSpeed);
		lastKnownSpeed = fanSpeed;
		client.sendMessage(stateTopic, "ON");
		digitalWrite(D1, HIGH);
	} else {
		client.sendMessage(stateTopic, "OFF");
		digitalWrite(D1, LOW);
	}
	//To see what's happening in the background
	//client.publish("debug", String("fanSpeed: " + String(fanSpeed) + ", lastKnownSpeed: " + String(lastKnownSpeed)).c_str());
}

unsigned int convectionRadiator::getFanSpeed(const unsigned int pin){
	return (1000000/pulseIn(pin, LOW))/4*60;
}

void convectionRadiator::turnOn(){
	setFanSpeed(lastKnownSpeed);
}

void convectionRadiator::turnOff(){
	setFanSpeed(0);
}

void convectionRadiator::updateAvailability(const bool stateAvailable){
	if(stateAvailable){
		client.sendMessage(availabilityTopic, "online"); 
	} else {
		client.sendMessage(availabilityTopic, "offline");
	}
}

void convectionRadiator::checkObstruction(){
	for(const unsigned int & pin : rpmPins){
		if(getFanSpeed(pin) < (16 * lastKnownSpeed / 10,23 + 500) - 800){
			turnOff();
			updateAvailability(false);
		} 
	}
}

bool convectionRadiator::turnedOn(){
	if(lastKnownSpeed > 150){
		return true;
	} else {
		return false;
	}
}

void convectionRadiator::checkButton(){
	buttonPressed = (digitalRead(D0) == HIGH);
	if(!buttonPressed && buttonPressed != lastButtonState){
		setFanSpeed(0);
		lastButtonState = buttonPressed;
	} else if(buttonPressed && buttonPressed != lastButtonState){
		lastButtonState = buttonPressed;
	}
}

void convectionRadiator::messageReceived(const String & receivedMessage){
	if(receivedMessage=="ON"){
        turnOn();
    } else if(receivedMessage=="OFF"){
        turnOff();
    } else if(receivedMessage.substring(0, 2) == "FS"){
        setFanSpeed(receivedMessage.substring(3).toInt()*10+23);
    } else if(receivedMessage == "CONNECTED"){
    	updateAvailability(true);
    } else if(receivedMessage == "DISCONNECTED"){
    	updateAvailability(false);
    }
}
