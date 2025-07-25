// 
// 
// 

#include "Motorhandling.h"

Motor::Motor(uint16_t BaseAddress, byte BaseChannel) :
	Accessory(BaseAddress, BaseChannel, 203),
	_motor(BaseChannel) {
	Serial.println("Motor::Motor(uint16_t BaseAddress, byte BaseChannel)");
}


//startdelay in ms
Motor::Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode, uint16_t startdelay) :
	Accessory(BaseAddress, BaseChannel, Mode),
	_motor(BaseChannel),
	_startdelay(startdelay) {
	Serial.println("Motor::Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode, uint16_t startdelay)");
	Serial.print("    Start delay: "); Serial.print(_startdelay); Serial.println("ms");
}

Motor::~Motor() {
	Serial.println("Motor::~Motor");
	_motor.off(); // Ensure the motor is turned off before destruction
}

AccessoryType Motor::getType() const {
	Serial.println("Motor::getType");
	return AccessoryType::Motor;
}

void Motor::SetMaxSpeed(uint8_t Speed) {
	if (Speed > 255) {
		Speed = 255; // Ensure speed does not exceed maximum value
	}
	_motor.setMaxSpeed(Speed);
	Serial.print("Motor::SetMaxSpeed: ");
	Serial.println(Speed);
}

void Motor::on() {
	Serial.println("Motor::on");
	Accessory::on();
	if (_startdelay > 0) {
		Serial.print("    Motor will start after delay of ");
		Serial.print(_startdelay);
		Serial.println(" ms");
		_timer.start(_startdelay);
		_startdelayTriggered = true;
	}
	else {
		_motor.on();
		_startdelayTriggered = false;
	}
}

void Motor::off() {
	Serial.println("Motor::off");
	Accessory::off();
	_motor.off();
	_startdelayTriggered = false;
}

void Motor::process() {
	Accessory::process();
	_motor.process();
	if (_timer.done() && _startdelayTriggered) {
		Serial.println("Motor::process: Timer done, turning on motor");
		_startdelayTriggered = false;
		_motor.on();
	}
}
