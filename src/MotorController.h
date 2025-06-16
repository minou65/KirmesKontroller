// MotorController.h

#ifndef _MOTORCONTROLLER_h
#define _MOTORCONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class MotorControl {

public:
	MotorControl(int pin, int pwmchannel, int frequency = 5000, int pwmResolution = 8);

	void startMotor();
	void stopMotor();
	void toggleMotor();
	bool isMotorRunning() const;

	void setMotorSpeed(int speed);
	void setMotorPWM(int pwm);
	void setMotorFrequency(int frequency);

private:
	int _pin;
	int _frequency;
	int _pwmResolution;
	int _pwmchannel;
	int _motorSpeed = 0; // 0-255 for PWM control
};

#endif

