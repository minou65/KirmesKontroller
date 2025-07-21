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
private:
    Servo _servo;

    Neotimer _intervalTimer;
	uint16_t _intervalSteps; // Number of steps in the current interval
	uint16_t _intervalTime; // Time in milliseconds for the current interval
    uint32_t _travelTime; // Total travel time between two limits in milliseconds

    uint8_t _GPIO;

    bool _IsActive;

    uint16_t _limit1;     // Limit of travel in one direction in gradients
    uint16_t _limit2;     // Limit of travel in the other direction in gradients
    uint16_t _tlimit1;    // Limit of travel in one directionin in tenths
    uint16_t _tlimit2;    // Limit of travel in the other direction in in tenths
	int32_t _targetThenths; // Target position in tenths
	int16_t _currentTenths; // Current position in tenths

    uint16_t _flags;	 // Configuration flags

    void setTravelTime(uint16_t ms);
    void SetIntervalTime(uint16_t interval);
    void SetIntervalTime(uint16_t travelTime, uint16_t minIntervalTime);

public:
    ServoControl() = default;
    ServoControl(int8_t ServoPort, int limit1, int limit2, int travelTime, unsigned int flags = SERVO_INITMID);
    ~ServoControl();
    void process();
	void on();
	void off();

    void setStartAngle(uint8_t degrees);
    void setEndAngle(uint8_t degrees);

    void setThenths(int tenths);
    void setPosition(int percentage);
    void setAngle(uint8_t degrees);

    uint32_t getTenths();
    uint8_t getAngle();

    void writeTenths(int tenths);
};

extern void notifyServoPosition(ServoControl*, int) __attribute__((weak));

#endif

