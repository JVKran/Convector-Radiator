#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSID";
const char* password = "WPA2";

unsigned long lastUpdate;
unsigned long lastFanCheck;
unsigned long lastButtonCheck;

char* mqttServer = "Broker Address";
String message;

WiFiClient espClient;
PubSubClient client(espClient);

class convectionRadiator{
  private:
    const unsigned int pwmPin;
    const int rpmPins[4] = {D5, D6, D7, D2};
    unsigned int lastKnownSpeed;
    bool buttonPressed = (digitalRead(D0) == HIGH);
    bool lastButtonState = false;
  public:
    convectionRadiator(const unsigned int pwmPin): pwmPin(pwmPin){}
    bool setFanSpeed(const unsigned int fanSpeed){
      if(fanSpeed == 0){
        analogWrite(pwmPin, fanSpeed);
        client.publish("/woonkamer/radiator/state", "OFF");
        digitalWrite(D1, LOW);
      } else if(buttonPressed){
        analogWrite(pwmPin, fanSpeed);
        lastKnownSpeed = fanSpeed;
        client.publish("/woonkamer/radiator/state", "ON");
        digitalWrite(D1, HIGH);
      } else {
        client.publish("/woonkamer/radiator/state", "OFF");
        digitalWrite(D1, LOW);
      }
      //To see what's happening in the background
      //client.publish("debug", String("fanSpeed: " + String(fanSpeed) + ", lastKnownSpeed: " + String(lastKnownSpeed)).c_str());
    }
    unsigned int getFanSpeed(const unsigned int pin){
      return (1000000/pulseIn(pin, LOW))/4*60;
    }
    void turnOn(){
      setFanSpeed(lastKnownSpeed);
    }
    void turnOff(){
      setFanSpeed(0);
    }
    void updateAvailability(const bool stateAvailable){
      if(stateAvailable){
        client.publish("/woonkamer/radiator/availability", "online"); 
      } else {
        client.publish("/woonkamer/radiator/availability", "offline");
      }
    }
    void checkObstruction(){
      for(const unsigned int & pin : rpmPins){
        if(getFanSpeed(pin) < (16 * lastKnownSpeed / 10,23 + 500) - 800){
          turnOff();
          updateAvailability(false);
        } 
      }
    }
    bool turnedOn(){
      if(lastKnownSpeed > 150){
        return true;
      } else {
        return false;
      }
    }
    void checkButton(){
      buttonPressed = (digitalRead(D0) == HIGH);
      if(!buttonPressed && buttonPressed != lastButtonState){
        setFanSpeed(0);
        lastButtonState = buttonPressed;
      } else if(buttonPressed && buttonPressed != lastButtonState){
        lastButtonState = buttonPressed;
      }
    }
};

class temperatureSensor{
  private:
    const unsigned int tempPin;
  public:
    temperatureSensor(const unsigned int tempPin): tempPin(tempPin) {}
    float getTemperature(){
      return float(((analogRead(tempPin)/1024.0)*3300) / 10);
    }
};

convectionRadiator fans(D3);
temperatureSensor tempSensor(A0);

void setup() {
  fans.setFanSpeed(0);
  pinMode(D5, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D0, INPUT);
  digitalWrite(D1, HIGH);
  analogWriteFreq(25000);
  WiFi.mode(WIFI_STA);
  setupWifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
  client.subscribe("/woonkamer/radiator");
  fans.setFanSpeed(0);
  fans.updateAvailability(true);
  digitalWrite(D1, LOW);
  client.publish("/woonkamer/radiator/debug", "started up");
}

void setupWifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ConvectorRadiator", "ConvectorClient", "Snip238!")) {
      client.subscribe("/woonkamer/radiator");
      fans.updateAvailability(true);
    } else {
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    message.concat((char)payload[i]);
  }
  if(message=="ON"){
    fans.turnOn();
  } else if(message=="OFF"){
    fans.turnOff();
  } else if(message.substring(0, 2) == "FS"){
    fans.setFanSpeed(message.substring(3).toInt()*10+23);
  }
  message = "";  
}

void updateSystem(){
  client.publish("/woonkamer/radiator/temperatuur", String(tempSensor.getTemperature()).c_str());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (millis() > lastUpdate + 30000){
    updateSystem();
    lastUpdate = millis();
  }
  if(millis() > lastFanCheck + 10000 && fans.turnedOn()){
    lastFanCheck = millis();
  } else if(millis() > lastFanCheck + 10000){
    lastFanCheck = millis();
  }
  if(millis() > lastButtonCheck + 1000){
    lastButtonCheck = millis();
    fans.checkButton();
  }
}
