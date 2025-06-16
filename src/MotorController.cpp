// 
// 
// 

#include <Arduino.h>
#include "MotorController.h"





MotorControl::MotorControl(int pin, int pwmchannel, int frequency, int pwmResolution):
	_pin(pin), 
	_pwmchannel(pwmchannel),
	_frequency(frequency),
	_pwmResolution(pwmResolution) {
	ledcAttachChannel(_pin, _frequency, _pwmResolution, _pwmchannel);
}

void MotorControl::startMotor(){
	setMotorSpeed(255);
}

void MotorControl::stopMotor(){
	setMotorSpeed(0);
}

void MotorControl::toggleMotor(){
	if (isMotorRunning()) {
		stopMotor();
	}
	else {
		startMotor();
	}
}

void MotorControl::setMotorSpeed(int speed) {
	if (speed < 0) speed = 0;
	if (speed > 255) speed = 255;
	_motorSpeed = speed;
	ledcWrite(_pwmchannel, _motorSpeed);
}

bool MotorControl::isMotorRunning() const{
	return _motorSpeed > 0;
}
