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
	accessories(BaseAddress, BaseChannel, 203),
	_motor(BaseChannel) {
	Serial.println("Motor::Motor(uint16_t BaseAddress, byte BaseChannel)");
}

Motor::Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode) :
	accessories(BaseAddress, BaseChannel, Mode),
	_motor(BaseChannel){
	Serial.println("Motor::Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode)");
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
	_motor.on();
}

void Motor::off() {
	Serial.println("Motor::off");
	_motor.off();
}

void Motor::process() {
	_motor.process();
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
	off();
}

void MotorControl::process() {
	LED::process();
}

void MotorControl::on(){
	_IsActive = true;
	ledcWrite(_Channel, _Speed);
}

void MotorControl::off(){
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
