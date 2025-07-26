// 
// 
// 

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#include "MotorControl.h"


MotorControl::MotorControl(const uint8_t pin) :
	_maxSpeed(255),
	_Speed(0),
	_GPIO(pin) {

	Serial.println("MotorControl::MotorControl");
	_Frequency = 5000; // Default frequency
	_Resolution = 8; // Default resolution
	Serial.print("    GPIO: "); Serial.println(_GPIO);
	attach(_GPIO);
	off();
}

MotorControl::MotorControl(const uint8_t pin, uint8_t Speed) :
	MotorControl(pin) {
	setMaxSpeed(Speed);
}

MotorControl::MotorControl(const uint8_t pin, const uint16_t frequency, const uint8_t resolution) :
	MotorControl(pin) {
	setFrequency(frequency);
	setPWM(resolution);
}

MotorControl::~MotorControl() {
	Serial.println("MotorControl::~MotorControl");
	detach();
	off();
}

bool MotorControl::attach(uint8_t pin) {
	if (_isAttached) {
		Serial.println("MotorControl::attach: already attached");
		return false;
	}
	_isAttached = ledcAttach(pin, _Frequency, _Resolution);
	if (_isAttached) {
		Serial.println("MotorControl::attach: success");
	} else {
		Serial.println("MotorControl::attach: failed");
	}
	return _isAttached;
};

void MotorControl::detach() {
	if (!_isAttached) {
		Serial.println("MotorControl::detach: not attached");
		return;
	}
	Serial.println("MotorControl::detach");
	ledcDetach(_GPIO);
	_isAttached = false;
};

void MotorControl::write(uint16_t value) {
	if (!_isAttached) {
		Serial.println("MotorControl::write: not attached");
		return;
	}
	if (value > _maxSpeed) {
		value = _maxSpeed;
	}
	ledcWrite(_GPIO, value);
	_Speed = value; // Update the current speed
	Serial.print("MotorControl::write: Speed set to ");
	Serial.println(_Speed);

};

void MotorControl::process() {
}

void MotorControl::on(){
	Serial.println("MotorControl::on");
	_IsActive = true;
	write(_Speed);
}

void MotorControl::off(){
	Serial.println("MotorControl::off");
	_IsActive = false;
	write(PWM_Set_Off);
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
	_Resolution = pwm;
	detach(); // Detach the pin before changing PWM settings

	////ledcDetach(_GPIO); // Detach the pin before changing PWM settings
	////ledcAttachChannel(_GPIO, _PWMFrequency, _PWMResolution, _Channel); // Attach the pin with the new settings
	//_Speed = map(_Speed, 0, 255, 0, (1 << _PWMResolution) - 1); // Map speed to PWM resolution
	//write(_Speed); // Update the PWM signal with the new resolution

}

void MotorControl::setFrequency(int frequency){
	//_PWMFrequency = frequency;
	////ledcDetach(_GPIO); // Detach the pin before changing PWM settings
	////ledcAttachChannel(_GPIO, _PWMFrequency, _PWMResolution, _Channel); // Attach the pin with the new settings
	//write(_Speed); // Update the PWM signal with the new frequency
}
