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
#include "ESPServo.h"

#define SERVO_INITL1		0x01
#define SERVO_INITL2		0x02
#define SERVO_INITMID		0x03

#define SERVO_ABSOLUTE		0x04
#define SERVO_BOUNCE_L1		0x08
#define SERVO_BOUNCE_L2		0x10
#define SERVO_AUTO_REVERSE	0x20
#define SERVO_REVERSE		0x80

#define SERVO_MIN() (544)  // minimum value in uS for this servo
#define SERVO_MAX() (2400)  // maximum value in uS for this servo 

class ServoControl : public Accessory {
private:
    ESPServo _espservo;
    Neotimer _TravelTimer;

    uint8_t _GPIO;
    uint8_t _Channel;

    bool _IsActive;

    int _limit1;     // Limit of travel in one direction
    int _limit2;     // Limit of travel in the other direction
    int _tlimit1;    // Limit of travel in one directionin tenths
    int _tlimit2;    // Limit of travel in the other direction in tenths
    long _travelTime; // Total travel time between two limits in milliseconds
    int _interval;   // Time between steps of 1 degree in milliseconds

    bool _clockwise;  // Current direction is clockwise
    int _percentage; // Current speed percentage
    bool _moving;     // Servo is moving

    int _angle;

    unsigned int _flags;	 // Configuration flags

    int _blimit;
    bool _bclockwise;
    bool _bounced;
    bool _bouncing;
    int _bouncingSteps;
    int _bouncingAngle;
    int _bouncepoint;

    int _reported;

public:
    ServoControl() = default;
    ServoControl(int8_t Channel, int limit1, int limit2, int travelTime, unsigned int flags = SERVO_INITMID);
    ~ServoControl();
    void process();
    void setActive(bool active);
    void setStart(int start);
    void setEnd(int angle);
    void setFlags(int flags);
    void setBounceAngle(int angle);
    void setBouncingSteps(int steps);
    void setPosition(int percentage);
    void setPosition(int percentage, bool clockwise);
    void setAngle(int angle);
    int getAngle();
    void setTravelTime(int time);
    void setTravelTime_ms(uint16_t ms);
    boolean isAbsolute();
    void writeTenths(int tenths);

    AccessoryType getType() const override { return AccessoryType::Servo; }
};

extern void notifyServoPosition(ServoControl*, int) __attribute__((weak));

#endif

