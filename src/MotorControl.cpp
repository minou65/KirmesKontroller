// 
// 
// 

#include <Arduino.h>
#include "MotorControl.h"

Motoraccessories::Motoraccessories(uint16_t BaseAddress, byte BaseChannel) :
	accessories(BaseAddress, BaseChannel) {
}

Motoraccessories::Motoraccessories(uint16_t BaseAddress, byte BaseChannel, byte Mode) :
	accessories(BaseAddress, BaseChannel, Mode) {
}

Motoraccessories::~Motoraccessories() {
}
AccessoryType Motoraccessories::getType() const {
	Serial.println("Motoraccessories::getType");
	return AccessoryType::Motor;
}

void Motoraccessories::SetMaxBrightness(uint16_t MaxBrightness) {
}


Motor::Motor(const uint8_t Channel) :
	_maxSpeed(255),
	_Speed(0) {

	Serial.println("Motor::Motor");
	LED(Channel);
}

Motor::Motor(const uint8_t Channel, uint8_t Speed) :
	Motor(Channel) {
	setMaxSpeed(Speed);
}

Motor::Motor(int pin, int Channel, int Frequency, int Resolution)
{
}

Motor::~Motor() {
	~LED();
}

void Motor::process() {
	LED::process();
}

void Motor::on(){
	_IsActive = true;
	ledcWrite(_Channel, _Speed);
}

void Motor::off(){
	_IsActive = false;
	ledcWrite(_Channel, (PWM_Set_Off);
}

void Motor::toggle(){
	if (isOn()) {
		off();
	}
	else {
		on();
	}
}

void Motor::setMaxSpeed(int speed) {
	if (PWM_Set_On == 0) {
		_maxSpeed = 255 - speed;
	}
	else {
		_maxSpeed = speed;
	}

	if (_Speed != 0)
		_Speed = _maxSpeed;;
}
