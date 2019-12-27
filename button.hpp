#ifndef __BUTTON_HPP
#define __BUTTON_HPP

#include "Arduino.h"

class buttonListener {
	public:
		virtual void buttonPressed() = 0;
		virtual void buttonReleased() = 0;
};

class button {
	private:
		const unsigned int buttonPin;
		bool buttonPressed;

		buttonListener * listeners[10];
		unsigned int amountOfListeners = 0;
	public:
		unsigned long lastCheck;
		button(const unsigned int buttonPin);

		void update();
		void addListener(buttonListener & listener);
};

#endif //__BUTTON_HPP