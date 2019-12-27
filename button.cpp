#include "button.hpp"

button::button(const unsigned int buttonPin):
	buttonPin(buttonPin)
{
	pinMode(buttonPin, INPUT);
	buttonPressed = (digitalRead(buttonPin) == HIGH);
}

void button::update(){
	if((digitalRead(buttonPin) == HIGH) != buttonPressed){
		buttonPressed = (digitalRead(buttonPin) == HIGH);
		for(uint8_t i = 0; i < amountOfListeners; i++){
			if(buttonPressed){
				listeners[i]->buttonPressed();
			} else {
				listeners[i]->buttonReleased();
			}
		}
	}
}

void button::addListener(buttonListener & listener){
	if(amountOfListeners < 8){
		listeners[amountOfListeners] = &listener;
		amountOfListeners++;
	}
}