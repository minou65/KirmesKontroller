#include "ServoControl.h"
#include "ESPServo.h"
#include "pinmapping.h"

ServoControl::ServoControl(int8_t Channel_, int limit1_, int limit2_, int travelTime_, unsigned int flags_) :
    GPIO(ChannelToGPIOMapping[Channel_]),
    Channel(Channel_),
    limit1(limit1_),
    limit2(limit2_),
    tlimit1(limit1_ * 10),
    tlimit2(limit2_ * 10),
    travelTime(travelTime_),
    angle(tlimit1),
    bouncing(false),
    bounced(false),
    bouncingSteps(1),
    bouncingAngle(0),
    clockwise(true),
    moving(false),
    IsActive(false),
    flags(flags_) {

   
    interval = travelTime / (max(tlimit1,tlimit2) - min(tlimit1, tlimit2));

    if (interval == 0)
        flags |= SERVO_ABSOLUTE;

    switch (flags & (SERVO_INITL1 | SERVO_INITL2 | SERVO_INITMID)){
    case SERVO_INITL1:
        espservo.attach(GPIO, Channel);
        espservo.write(limit1);
        break;
    case SERVO_INITL2:
        espservo.attach(GPIO, Channel);
        espservo.write(limit2);
        break;
    case SERVO_INITMID:
        espservo.attach(GPIO, Channel);
        espservo.write(limit1 + (limit2 - limit1) / 2);
        break;
    }

    Serial.println("ServoControl::ServoControl");
    Serial.print("    Channel:    "); Serial.println(Channel);
    Serial.print("    GPIO:       "); Serial.println(GPIO);
    Serial.print("    TravelTime: "); Serial.println(travelTime);
    Serial.print("    Limit1:     "); Serial.println(limit1);
    Serial.print("    Limit2:     "); Serial.println(limit2);
    Serial.print("    tLimit1:    "); Serial.println(tlimit1);
    Serial.print("    tLimit2:    "); Serial.println(tlimit2);
    Serial.print("    Interval:   "); Serial.println(interval);
    Serial.print("    Angle:      "); Serial.println(angle);
    Serial.print("    Flags:      "); Serial.println(flags);
}

ServoControl::~ServoControl() {
    setActive(false);
    espservo.~ESPServo();
    TravelTimer.~Neotimer();
}

void ServoControl::process() {
    int16_t  _newangle;
    bool _clockwise;

    if (flags & SERVO_ABSOLUTE) {
        if (TravelTimer.done()) {
            setActive(false);
        }
        return;
    }

    if (!TravelTimer.repeat())
        return;

    if (IsActive == false || moving == false) {
        TravelTimer.start(interval);
        return;
    }

    _clockwise = clockwise;
    if (flags & SERVO_REVERSE)
        _clockwise = !_clockwise;

    _newangle = angle;
    if (bouncing) {
        if (bclockwise && angle < blimit) {
            _newangle = angle + 1;
        }
        else if (bclockwise == false && angle > bouncepoint) {
            _newangle = angle - 1;
        }
        else if (angle == bouncepoint) {
            bouncingSteps++;
            int _bouncingAngle;
            _bouncingAngle = 10 * (bouncingAngle >> (bouncingSteps / 2));
            if (_bouncingAngle == 0) {
                bouncing = false;
                bounced = true;
                _newangle = tlimit2;
            }
            else {
                if (blimit == tlimit2)
                    bouncepoint = tlimit2 - (10 * _bouncingAngle);
                else
                    blimit = tlimit1 + (10 * _bouncingAngle);
                bclockwise = !bclockwise;
            }
        }
        else {
            bouncingSteps++;
            int _bouncingAngle;
            _bouncingAngle = 10 * (bouncingAngle >> (bouncingSteps / 2));
            if (_bouncingAngle == 0) {
                bouncing = false;
                bounced = true;
                _newangle = tlimit1;
            }
            else {
                if (blimit == tlimit2)
                    bouncepoint = tlimit2 - (10 * _bouncingAngle);
                else
                    blimit = tlimit1 + (10 * _bouncingAngle);
                bclockwise = !bclockwise;
            }
        }
    }
    else {
        if (clockwise && angle < tlimit2) {
            _newangle = angle + 1;
            bounced = false;
        }
        else if (clockwise == false && angle > tlimit1) {
            _newangle = angle - 1;
            bounced = false;
        }
        else if ((flags & SERVO_BOUNCE_L1) != 0 && clockwise == false
            && bounced == false && bouncingAngle != 0) {
            bouncing = true;
            bouncepoint = angle;
            blimit = tlimit1 + (10 * bouncingAngle);
            bclockwise = true;
        }
        else if ((flags & SERVO_BOUNCE_L2) != 0 && clockwise
            && bounced == false && bouncingAngle != 0) {
            bouncing = true;
            blimit = angle;
            bouncepoint = tlimit2 - (10 * bouncingAngle);
            bclockwise = false;
        }
        else if (flags & SERVO_AUTO_REVERSE)
            flags = flags ^ SERVO_REVERSE;
        else
            moving = false;
    }

    if (_newangle != angle) {
        writeTenths(_newangle);
        angle = _newangle;
    }
    if (angle / 10 != reported) {
        reported = angle / 10;
        if (notifyServoPosition)
            notifyServoPosition(this, reported);
    } 

    if (!moving) {
        setActive(false);
    }
}

void ServoControl::setActive(bool active_){
    Serial.println("ServoControl::setActive"),
    Serial.print("    active_: "); Serial.println(active_ ? "true" : "false");
        
    if ((active_ == true) && (IsActive == false)){
        if (espservo.attach(GPIO, Channel)) {
            Serial.println("    pin attached to channel");
            TravelTimer.start(interval);
        }
    }
    else if ((active_ == false) && (IsActive == true)) {

        espservo.detach();
        TravelTimer.stop();
        Serial.println("    pin detattached from channel");
    }
    IsActive = active_;
}

void ServoControl::setStart(int start_){
    if (start_ < 0)
        start_ = 0;
    else if (start_ > 180)
        start_ = 180;
    limit1 = start_;
    tlimit1 = start_ * 10;
    interval = travelTime / (max(tlimit1, tlimit2) - min(tlimit1, tlimit2));
    if (interval > 0) {
        flags &= ~SERVO_ABSOLUTE;
    }
    else {
        flags |= SERVO_ABSOLUTE;
    }
}

void ServoControl::setEnd(int angle_){
    if (angle_ < 0)
        angle_ = 0;
    else if (angle_ > 180)
        angle_ = 180;
    limit2 = angle_;
    tlimit2 = angle_ * 10;
    interval = travelTime / (max(tlimit1, tlimit2) - min(tlimit1, tlimit2));
    if (interval > 0) {
        flags &= ~SERVO_ABSOLUTE;
    }
    else {
        flags |= SERVO_ABSOLUTE;
    }
}

void ServoControl::setFlags(int flags_){
    flags = (int8_t)flags_;
}

void ServoControl::setBounceAngle(int angle_){
    bouncingAngle = angle_;
}

void ServoControl::setBouncingSteps(int steps_){
    bouncingSteps = steps_;
}

void ServoControl::setPosition(int percentage_) {
    uint32_t _range = max(tlimit1, tlimit2) - min(tlimit1, tlimit2);
    uint32_t _tenth = (percentage_ * _range) / 100;
    
    setActive(true);

    _tenth += tlimit1;
    if (IsActive){
        angle = _tenth;
        writeTenths(_tenth);
        TravelTimer.start(2000);
    }
}

void ServoControl::setPosition(int percentage_, bool clockwise_) {
    Serial.println("ServoControl::setSpeed");
    Serial.print("    Percent:   "); Serial.println(percentage_);
    Serial.print("    clockwise: "); Serial.println(clockwise_ ? "true" : "false");

    percentage = percentage_;
    clockwise = clockwise_;
    moving = (percentage > 0);

    setActive(moving);
}

void ServoControl::setAngle(int angle_) {
    uint16_t _tenth = angle_ * 10;

    angle = _tenth;
    if (IsActive) {
        writeTenths(_tenth);
    }
}

int ServoControl::getAngle() {
    return angle / 10;
}

void ServoControl::setTravelTime(int time_) {
    travelTime = time_ * 1000;
    interval = travelTime / (tlimit2 - tlimit1);
}

boolean ServoControl::isAbsolute() {
    if (flags & SERVO_ABSOLUTE)
        return true;
    return false;
}

void ServoControl::writeTenths(int tenth_) {
    Serial.println("ServoControl::writeTenths");
    Serial.print("    tenths: "); Serial.println(tenth_);

    espservo.writeMicroseconds(map(tenth_, 0, 1800, SERVO_MIN(), SERVO_MAX()));
}


