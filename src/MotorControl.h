// MotorController.h

#ifndef _MOTORCONTROL_h
#define _MOTORCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "LEDControl.h"

class Motoraccessories : public accessories {
public:
	Motoraccessories() = default;
	Motoraccessories(uint16_t BaseAddress, byte BaseChannel);
	Motoraccessories(uint16_t BaseAddress, byte BaseChannel, byte Mode);
	~Motoraccessories();
	AccessoryType getType() const override;
	virtual void SetMaxBrightness(uint16_t MaxBrightness);
};

class Motor : public LED{

public:
    Motor(const uint8_t Channel);
    Motor(const uint8_t Channel, uint8_t Speed);
    Motor(int pin, int Channel, int Frequency, int Resolution);
    ~Motor();

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


#endif

