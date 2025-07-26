// MotorController.h

#ifndef _MOTORCONTROL_h
#define _MOTORCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
	#include "WProgram.h"
#endif

#include "LEDControl.h"

class MotorControl {

public:
    MotorControl(const uint8_t pin);
    MotorControl(const uint8_t pin, uint8_t Speed);
    MotorControl(const uint8_t pin, const uint16_t frequency, const uint8_t resolution);
    ~MotorControl();

	bool attach(uint8_t pin);
	void detach();
	void write(uint16_t value);

    void process();

    void on();
    void off();
    void toggle();

	bool isOn() const { return _IsActive; }

    void setMaxSpeed(int speed);
    void setPWM(int pwm);
    void setFrequency(int frequency);

private:
	uint8_t _GPIO;
	bool _IsActive;

    int _Speed; // 0-255 for PWM control
    int _maxSpeed;
    bool _isOn = false;

    bool _isAttached = false;

    uint16_t _Frequency;
    uint16_t _Resolution;
};






#endif

