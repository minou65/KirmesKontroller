// MotorController.h

#ifndef _MOTORCONTROL_h
#define _MOTORCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "LEDControl.h"

class MotorControl : public LED {

public:
    MotorControl(const uint8_t Channel);
    MotorControl(const uint8_t Channel, uint8_t Speed);
    MotorControl(const uint8_t Channel, const uint16_t Frequency, const uint8_t Resolution);
    ~MotorControl();

    void process();

    void on();
    void off();
    void toggle();

    void setMaxSpeed(int speed);
    void setPWM(int pwm);
    void setFrequency(int frequency);

private:
    int _Speed; // 0-255 for PWM control
    int _maxSpeed;
    bool _isOn = false;
};

class Motor : public accessories {
public:
    Motor() = default;
    Motor(uint16_t BaseAddress, byte BaseChannel);
    Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode);
	~Motor();
	AccessoryType getType() const override;
	virtual void SetMaxSpeed(uint8_t Speed);

    void on();
	void off();
	void process();

private:
	MotorControl _motor;

};




#endif

