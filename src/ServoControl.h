// Servos.h

#ifndef _SERVOS_h
#define _SERVOS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "neotimer.h"
#include "ESP32Servo.h"

#define SERVO_INITL1		0x01
#define SERVO_INITL2		0x02
#define SERVO_INITMID		0x03

#define SERVO_ABSOLUTE		0x04
#define SERVO_BOUNCE_L1		0x08
#define SERVO_BOUNCE_L2		0x10
#define SERVO_AUTO_REVERSE	0x20
#define SERVO_REVERSE		0x80

#define SERVO_MIN() (500)  // minimum value in uS for this servo
#define SERVO_MAX() (2500)  // maximum value in uS for this servo 

class ServoControl {
protected:
    Servo _servo;

    Neotimer _intervalTimer;
	uint16_t _intervalSteps; // Number of steps in the current interval
	uint16_t _intervalTime; // Time in milliseconds for the current interval
    uint32_t _travelTime; // Total travel time between two limits in milliseconds

    uint8_t _GPIO;

    uint16_t _tlimit1;    // Limit of travel in one directionin in tenths
    uint16_t _tlimit2;    // Limit of travel in the other direction in in tenths
	int32_t _targetTenths; // Target position in tenths
	int16_t _currentTenths; // Current position in tenths

    uint16_t _flags;	 // Configuration flags

    void setTravelTime(uint16_t ms);
    void setIntervalTime(uint16_t interval);
    void setIntervalTime(uint16_t travelTime, uint16_t minIntervalTime);

public:
    ServoControl() = default;
    ServoControl(int8_t ServoPort, int limit1, int limit2, int travelTime, unsigned int flags = SERVO_INITMID | SERVO_BOUNCE_L1 | SERVO_BOUNCE_L2);
    ~ServoControl();
   
    virtual void process();
	virtual void on();
	virtual void off();

    void setLimit(int limit1, int limit2);

    void setTenths(int tenths);
    void setPercentPosition(int percentage);
    void setAngle(uint8_t degrees);

    uint32_t getTenths();
    uint8_t getAngle();

	bool isMoving() const { return _currentTenths != _targetTenths; }
	bool isClockwise() const { return _targetTenths > _currentTenths; }

    void writeTenths(int tenths);
};

class ServoBounce : public ServoControl {
public:
    ServoBounce() = default;
    ServoBounce(int8_t ServoPort, int limit1, int limit2, int travelTime, unsigned int flags = SERVO_INITMID);
    ~ServoBounce() = default;
    void process() override;
    void on() override;
    void off() override;

    void getBounceLimit(int limit1, int limit2);

	void setBouncingLimit(int tenths) { _bounceLimit = tenths; }
	void setBouncingTime(uint16_t time) { _bounceTime = time; }
	void setBouncingCount(uint8_t count) { _bounceCount = count; }
    	
	uint16_t getBounceLimit() const { return _bounceLimit; }
	uint16_t getBounceTime() const { return _bounceTime; }
    uint8_t getBounceCount() const { return _bounceCount; }

private:
    bool _bounceDirection; // true = towards limit1, false = towards limit2
    uint16_t _bounceTime; // Time in milliseconds for bouncing
    Neotimer _bounceTimer; // Timer for bouncing    
    uint16_t _bounceLimit; // Bounce limit in tenths
    uint8_t _bounceCount = 0; // Count of bounces

    bool _shouldBounce = false;
	bool _bounceStarted = false;

};

extern void notifyServoPosition(ServoControl*, int) __attribute__((weak));

#endif

