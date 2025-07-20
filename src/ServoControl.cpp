#include "ServoControl.h"
#include "ESPServo.h"
#include "pinmapping.h"

ServoControl::ServoControl(int8_t Channel_, int limit1_, int limit2_, int travelTime_, unsigned int flags_) :
    _GPIO(ServoChannelToGPIOMapping[Channel_]),
    _Channel(Channel_),
    _limit1(limit1_),
    _limit2(limit2_),
    _tlimit1(limit1_ * 10),
    _tlimit2(limit2_ * 10),
    _travelTime(travelTime_),
    _angle(_tlimit1),
    _bouncing(false),
    _bounced(false),
    _bouncingSteps(1),
    _bouncingAngle(0),
    _clockwise(true),
    _moving(false),
    _IsActive(false),
    _flags(flags_) {

   
    _interval = _travelTime / (max(_tlimit1, _tlimit2) - min(_tlimit1, _tlimit2));

    if (_interval == 0)
        _flags |= SERVO_ABSOLUTE;

    switch (_flags & (SERVO_INITL1 | SERVO_INITL2 | SERVO_INITMID)){
    case SERVO_INITL1:
        _espservo.attach(_GPIO, _Channel);
        _espservo.write(_limit1);
        break;
    case SERVO_INITL2:
        _espservo.attach(_GPIO, _Channel);
        _espservo.write(_limit2);
        break;
    case SERVO_INITMID:
        _espservo.attach(_GPIO, _Channel);
        _espservo.write(_limit1 + (_limit2 - _limit1) / 2);
        break;
    }

    Serial.println("ServoControl::ServoControl");
    Serial.print("    Channel:    "); Serial.println(_Channel);
    Serial.print("    GPIO:       "); Serial.println(_GPIO);
    Serial.print("    TravelTime: "); Serial.println(_travelTime);
    Serial.print("    Limit1:     "); Serial.println(_limit1);
    Serial.print("    Limit2:     "); Serial.println(_limit2);
    Serial.print("    tLimit1:    "); Serial.println(_tlimit1);
    Serial.print("    tLimit2:    "); Serial.println(_tlimit2);
    Serial.print("    Interval:   "); Serial.println(_interval);
    Serial.print("    Angle:      "); Serial.println(_angle);
    Serial.print("    Flags:      "); Serial.println(_flags);
}

ServoControl::~ServoControl() {
    setActive(false);
    _espservo.~ESPServo();
    _TravelTimer.~Neotimer();
}

void ServoControl::process() {
    int16_t  newAngle_;
    bool clockwise_;

    if (_flags & SERVO_ABSOLUTE) {
        if (_TravelTimer.done()) {
            setActive(false);
        }
        return;
    }

    if (!_TravelTimer.repeat())
        return;

    if (_IsActive == false || _moving == false) {
        _TravelTimer.start(_interval);
        return;
    }

    clockwise_ = _clockwise;
    if (_flags & SERVO_REVERSE)
        clockwise_ = !clockwise_;

    newAngle_ = _angle;
    if (_bouncing) {
        if (_bclockwise && _angle < _blimit) {
            newAngle_ = _angle + 1;
        }
        else if (_bclockwise == false && _angle > _bouncepoint) {
            newAngle_ = _angle - 1;
        }
        else if (_angle == _bouncepoint) {
            _bouncingSteps++;
            int _bouncingAngle;
            _bouncingAngle = 10 * (_bouncingAngle >> (_bouncingSteps / 2));
            if (_bouncingAngle == 0) {
                _bouncing = false;
                _bounced = true;
                newAngle_ = _tlimit2;
            }
            else {
                if (_blimit == _tlimit2)
                    _bouncepoint = _tlimit2 - (10 * _bouncingAngle);
                else
                    _blimit = _tlimit1 + (10 * _bouncingAngle);
                _bclockwise = !_bclockwise;
            }
        }
        else {
            _bouncingSteps++;
            int _bouncingAngle;
            _bouncingAngle = 10 * (_bouncingAngle >> (_bouncingSteps / 2));
            if (_bouncingAngle == 0) {
                _bouncing = false;
                _bounced = true;
                newAngle_ = _tlimit1;
            }
            else {
                if (_blimit == _tlimit2)
                    _bouncepoint = _tlimit2 - (10 * _bouncingAngle);
                else
                    _blimit = _tlimit1 + (10 * _bouncingAngle);
                _bclockwise = !_bclockwise;
            }
        }
    }
    else {
        if (clockwise_ && _angle < _tlimit2) {
            newAngle_ = _angle + 1;
            _bounced = false;
        }
        else if (_clockwise == false && _angle > _tlimit1) {
            newAngle_ = _angle - 1;
            _bounced = false;
        }
        else if ((_flags & SERVO_BOUNCE_L1) != 0 && clockwise_ == false
            && _bounced == false && _bouncingAngle != 0) {
            _bouncing = true;
            _bouncepoint = _angle;
            _blimit = _tlimit1 + (10 * _bouncingAngle);
            _bclockwise = true;
        }
		else if ((_flags & SERVO_BOUNCE_L2) != 0 && clockwise_ == true
            && _bounced == false && _bouncingAngle != 0) {
            _bouncing = true;
            _blimit = _angle;
            _bouncepoint = _tlimit2 - (10 * _bouncingAngle);
            _bclockwise = false;
        }
        else if (_flags & SERVO_AUTO_REVERSE)
            _flags = _flags ^ SERVO_REVERSE;
        else
            _moving = false;
    }

    if (newAngle_ != _angle) {
        writeTenths(newAngle_);
        _angle = newAngle_;
    }
    if (_angle / 10 != _reported) {
        _reported = _angle / 10;
        if (notifyServoPosition)
            notifyServoPosition(this, _reported);
    } 

    if (!_moving) {
        setActive(false);
    }
}

void ServoControl::setActive(bool active){
    Serial.println("ServoControl::setActive");
    Serial.print("    active: "); Serial.println(active ? "true" : "false");
        
    if ((active == true) && (_IsActive == false)){
        if (_espservo.attach(_GPIO, _Channel)) {
            Serial.println("    pin attached to channel");
            _TravelTimer.start(_interval);
        }
    }
    else if ((active == false) && (_IsActive == true)) {

        _espservo.detach();
        _TravelTimer.stop();
        Serial.println("    pin detattached from channel");
    }
    _IsActive = active;
}

void ServoControl::setStart(int start){
    if (start < 0)
        start = 0;
    else if (start > 180)
        start = 180;
    _limit1 = start;
    _tlimit1 = start * 10;
    _interval = _travelTime / (max(_tlimit1, _tlimit2) - min(_tlimit1, _tlimit2));
    if (_interval > 0) {
        _flags &= ~SERVO_ABSOLUTE;
    }
    else {
        _flags |= SERVO_ABSOLUTE;
    }
}

void ServoControl::setEnd(int Angle){
    if (Angle < 0)
        Angle = 0;
    else if (Angle > 180)
        Angle = 180;
    _limit2 = Angle;
    _tlimit2 = Angle * 10;
    _interval = _travelTime / (max(_tlimit1, _tlimit2) - min(_tlimit1, _tlimit2));
    if (_interval > 0) {
        _flags &= ~SERVO_ABSOLUTE;
    }
    else {
        _flags |= SERVO_ABSOLUTE;
    }
}

void ServoControl::setFlags(int flags){
    _flags = (int8_t)flags;
}

void ServoControl::setBounceAngle(int Angle){
    _bouncingAngle = Angle;
}

void ServoControl::setBouncingSteps(int steps){
    _bouncingSteps = steps;
}

void ServoControl::setPosition(int percentage) {
    uint32_t range_ = max(_tlimit1, _tlimit2) - min(_tlimit1, _tlimit2);
    uint32_t tenth_ = (percentage * range_) / 100;
    
    setActive(true);

    tenth_ += _tlimit1;
    if (_IsActive){
        _angle = tenth_;
        writeTenths(tenth_);
        _TravelTimer.start(2000);
    }
}

void ServoControl::setPosition(int percentage, bool clockwise) {
    Serial.println("ServoControl::setSpeed");
    Serial.print("    Percent:   "); Serial.println(percentage);
    Serial.print("    clockwise: "); Serial.println(clockwise ? "true" : "false");

    _percentage = percentage;
    _clockwise = clockwise;
    _moving = (_percentage > 0);

    setActive(_moving);
}

void ServoControl::setAngle(int angle) {
    uint16_t tenth_ = angle * 10;

    _angle = tenth_;
    if (_IsActive) {
        writeTenths(tenth_);
    }
}

int ServoControl::getAngle() {
    return _angle / 10;
}

// Set the travel time in seconds and calculate the interval based on the limits
// only for backwards compatibility
// this function will be removed in the future
// it is recommended to use setTravelTime_ms() instead
void ServoControl::setTravelTime(int time) {
	setTravelTime_ms(time * 1000);
}

// Set the travel time in milliseconds and calculate the interval based on the limits
void ServoControl::setTravelTime_ms(uint16_t ms) {
    _travelTime = ms;
    _interval = _travelTime / (_tlimit2 - _tlimit1);
}

boolean ServoControl::isAbsolute() {
    if (_flags & SERVO_ABSOLUTE)
        return true;
    return false;
}

void ServoControl::writeTenths(int tenths) {
    Serial.println("ServoControl::writeTenths");
    Serial.print("    tenths: "); Serial.println(tenths);

    _espservo.writeMicroseconds(map(tenths, 0, 1800, SERVO_MIN(), SERVO_MAX()));
}


