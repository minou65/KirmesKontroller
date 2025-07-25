// Servohandling.h

#ifndef _SERVOHANDLING_h
#define _SERVOHANDLING_h

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

	void MoveServo(uint16_t percentage);

	uint16_t getServoTenthsPosition() const {
		if (_servoControl) {
			return _servoControl->getTenths();
		}
		return 0;
	}

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
	ServoPendelAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime, uint16_t onTime, uint16_t offTime);
	~ServoPendelAccessory();
	AccessoryType getType() const override { return AccessoryType::ServoPendel; }
	void process() override;
	void on() override;
	void off() override;
private:
	Neotimer _timerPendel;
	uint16_t _onTime = 0; // Time in milliseconds for the pendulum to stay in one position
	uint16_t _offTime = 0; // Time in milliseconds for the pendulum to stay in one position
	bool _started = false; // Flag to indicate if the pendulum has started moving

	bool _direction = false; // true = clockwise, false = counter-clockwise
};


#endif

