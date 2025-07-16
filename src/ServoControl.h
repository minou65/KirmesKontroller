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
    ESPServo espservo;
    Neotimer TravelTimer;

    uint8_t GPIO;
    uint8_t Channel;

    bool IsActive;

    int limit1;     // Limit of travel in one direction
    int limit2;     // Limit of travel in the other direction
    int tlimit1;    // Limit of travel in one directionin tenths
    int tlimit2;    // Limit of travel in the other direction in tenths
    long travelTime; // Total travel time between two limits in milliseconds
    int interval;   // Time between steps of 1 degree in milliseconds

    bool clockwise;  // Current direction is clockwise
    int percentage; // Current speed percentage
    bool moving;     // Servo is moving

    int angle;

    unsigned int flags;	 // Configuration flags

    int blimit;
    bool bclockwise;
    bool bounced;
    bool bouncing;
    int bouncingSteps;
    int bouncingAngle;
    int bouncepoint;

    int reported;

public:
    ServoControl() = default;
    ServoControl(int8_t Channel_, int limit1_, int limit2_, int travelTime_, unsigned int flags_ = SERVO_INITMID);
    ~ServoControl();
    void process();
    void setActive(bool active_);
    void setStart(int start_);
    void setEnd(int angle_);
    void setFlags(int flags_);
    void setBounceAngle(int angle_);
    void setBouncingSteps(int steps_);
    void setPosition(int percentage_);
    void setPosition(int percentage_, bool clockwise_);
    void setAngle(int angle_);
    int getAngle();
    void setTravelTime(int time_);
    boolean isAbsolute();
    void writeTenths(int tenth_);

    AccessoryType getType() const override { return AccessoryType::Servo; }
};

extern void notifyServoPosition(ServoControl*, int) __attribute__((weak));

#endif

