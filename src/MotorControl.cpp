// 
// 
// 

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#include "MotorControl.h"

Motor::Motor(uint16_t BaseAddress, byte BaseChannel) :
	Accessory(BaseAddress, BaseChannel, 203),
	_motor(BaseChannel) {
	Serial.println("Motor::Motor(uint16_t BaseAddress, byte BaseChannel)");
}


//startdelay in ms
Motor::Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode, uint16_t startdelay) :
	Accessory(BaseAddress, BaseChannel, Mode),
	_motor(BaseChannel),
	_startdelay(startdelay){
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

MotorControl::MotorControl(const uint8_t Channel) :
	_maxSpeed(255),
	_Speed(0),
	LED(Channel, PWM_Set_Off) {

	Serial.println("MotorControl::MotorControl");
	off();
}

MotorControl::MotorControl(const uint8_t Channel, uint8_t Speed) :
	MotorControl(Channel) {
	setMaxSpeed(Speed);
}

MotorControl::MotorControl(const uint8_t Channel, const uint16_t Frequency, const uint8_t Resolution) :
	MotorControl(Channel) {
	setFrequency(Frequency);
	setPWM(Resolution);
}

MotorControl::~MotorControl() {
	Serial.println("MotorControl::~MotorControl");
	off();
}

void MotorControl::process() {
	LED::process();
}

void MotorControl::on(){
	Serial.println("MotorControl::on");
	_IsActive = true;
	ledcWrite(_Channel, _Speed);
}

void MotorControl::off(){
	Serial.println("MotorControl::off");
	_IsActive = false;
	ledcWrite(_Channel, PWM_Set_Off);
}

void MotorControl::toggle(){
	if (isOn()) {
		off();
	}
	else {
		on();
	}
}

void MotorControl::setMaxSpeed(int speed) {
	if (PWM_Set_On == 0) {
		_maxSpeed = 255 - speed;
	}
	else {
		_maxSpeed = speed;
	}

	if (_Speed != 0)
		_Speed = _maxSpeed;;
}

void MotorControl::setPWM(int pwm){
	_PWMResolution = pwm;
	ledcDetach(_GPIO); // Detach the pin before changing PWM settings
	ledcAttachChannel(_GPIO, _PWMFrequency, _PWMResolution, _Channel); // Attach the pin with the new settings
	_Speed = map(_Speed, 0, 255, 0, (1 << _PWMResolution) - 1); // Map speed to PWM resolution
	ledcWrite(_Channel, _Speed); // Update the PWM signal with the new resolution

}

void MotorControl::setFrequency(int frequency){
	_PWMFrequency = frequency;
	ledcDetach(_GPIO); // Detach the pin before changing PWM settings
	ledcAttachChannel(_GPIO, _PWMFrequency, _PWMResolution, _Channel); // Attach the pin with the new settings
	ledcWrite(_Channel, _Speed); // Update the PWM signal with the new frequency
}
