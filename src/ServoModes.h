// ServoModes.h

#ifndef _SERVOMODES_h
#define _SERVOMODES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ServoControl.h"
#include "accessories.h"
#include "neotimer.h"

class ServoAccessory : public Accessory {
public:
	ServoAccessory() = default;
	ServoAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime);
	~ServoAccessory();
	AccessoryType getType() const override { return AccessoryType::Servo; }
	void process() override;
	void on() override;
	void off() override;

	void MoveServo(uint16_t percentage, bool clockwise);

private:
	int8_t _ServoPort = -1;
	int16_t _limit1 = 0;
	int16_t _limit2 = 0;
	int16_t _travelTime = 0;
	ServoControl* _servoControl = nullptr;

	
};

class ServoImpulseAccessory : public ServoAccessory {
public:
	ServoImpulseAccessory() = default;
	ServoImpulseAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime, uint16_t impulsTime);
	~ServoImpulseAccessory();
	AccessoryType getType() const override { return AccessoryType::ServoImpulse; }
	void process() override;
	void on() override;
	void off() override;

private:
	uint16_t _impulsTime = 0; // Time in milliseconds for the impulse
	Neotimer _timer;
};

class ServoFlipAccessory : public ServoAccessory {
public:
	ServoFlipAccessory() = default;
	ServoFlipAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime);
	~ServoFlipAccessory();
	AccessoryType getType() const override { return AccessoryType::ServoFlip; }
	void process() override;
	void on() override;
	void off() override;
};

class ServoPendelAccessory : public ServoAccessory {
public:
	ServoPendelAccessory() = default;
	ServoPendelAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime, uint16_t pendelTime);
	~ServoPendelAccessory();
	AccessoryType getType() const override { return AccessoryType::ServoPendel; }
	void process() override;
	void on() override;
	void off() override;
private:
	Neotimer _timer;
	uint16_t _pendelTime = 0; // Time in milliseconds for the pendulum movement

	bool _direction = true; // true = clockwise, false = counter-clockwise
	uint16_t _currentPosition = 0; // Current position of the servo in percentage
	uint16_t _stepSize = 5; // Step size for pendulum movement in percentage
};


#endif

