// Motorhandling.h

#ifndef _MOTORHANDLING_h
#define _MOTORHANDLING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "MotorControl.h"

class Motor : public Accessory {
public:
	Motor() = default;
	Motor(uint16_t BaseAddress, byte BaseChannel);
	Motor(uint16_t BaseAddress, byte BaseChannel, byte Mode, uint16_t startdelay);
	~Motor();
	AccessoryType getType() const override;
	virtual void SetMaxSpeed(uint8_t Speed);

	void on() override;
	void off() override;
	void process() override;

private:
	uint16_t _startdelay;
	MotorControl _motor;
	Neotimer _timer;
	bool _startdelayTriggered = false;
};

#endif

