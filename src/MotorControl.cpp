// 
// 
// 

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#include "MotorControl.h"


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
