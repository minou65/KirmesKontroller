#include "ServoControl.h"
#include "pinmapping.h"

uint16_t setFlag(uint16_t flag, uint16_t value) {
    return flag | value;
}

uint16_t removeFlag(uint16_t flag, uint16_t value) {
    return flag & ~value;
}

bool isFlag(uint16_t flag, uint16_t value) {
    return (flag & value) != 0;
}

ServoControl::ServoControl(int8_t ServoPort, int limit1, int limit2, int travelTime_, unsigned int flags) :
    _GPIO(ServoPort),
    _travelTime(travelTime_),
    _flags(flags) {

    Serial.println("ServoControl::ServoControl");
	Serial.print("    travelTime: "); Serial.println(_travelTime);
	Serial.print("    flags:      "); Serial.println(_flags, HEX);

    setLimit(limit1, limit2);
    setIntervalTime(_travelTime, 10);

    Serial.print("    tLimit1:    "); Serial.println(_tlimit1);
    Serial.print("    tLimit2:    "); Serial.println(_tlimit2);

	uint16_t tmid_ = (abs(_tlimit2 - _tlimit1) / 2) + _tlimit1;

    if (isFlag(_flags, SERVO_INITL1)) {
		Serial.println("    Initializing to limit 1");
		Serial.print("    Limit 1 Tenths: "); Serial.println(_tlimit1);
        _currentTenths = _tlimit1;
        _targetTenths = _tlimit1;
    } else if (isFlag(_flags, SERVO_INITL2)) {
		Serial.println("    Initializing to limit 2");
		Serial.print("    Limit 2 Tenths: "); Serial.println(_tlimit2);
        _currentTenths = _tlimit2;
        _targetTenths = _tlimit2;
    } else if (isFlag(_flags, SERVO_INITMID)) {
		Serial.println("    Initializing to mid position");
		Serial.print("    Mid Position Tenths: "); Serial.println(tmid_);
        _currentTenths = tmid_;
        _targetTenths = _currentTenths;
    } else {
        // Default to mid position if no specific initialization flag is set
		Serial.println("    Initializing to mid position (default)");
		Serial.print("    Mid Position Tenths: "); Serial.println(tmid_);
        _currentTenths = tmid_;
        _targetTenths = _currentTenths;
	}

    _servo.setPeriodHertz(50);
    _servo.attach(_GPIO, SERVO_MIN(), SERVO_MAX());
	writeTenths(_currentTenths); // Initialize servo position
}

ServoControl::~ServoControl() {
	_servo.detach();
    _servo.~Servo();
    _intervalTimer.~Neotimer();
}


void ServoControl::process() {
    int16_t direction_;

    if (_currentTenths != _targetTenths) {
        direction_ = _targetTenths - _currentTenths;

        if (_intervalTimer.repeat() || true) {
            float stepSize = (float)abs(_tlimit2 - _tlimit1) / ((float)_travelTime / (float)_intervalTime);
            _stepAccumulator += stepSize;

            int stepsToMove = (int)_stepAccumulator;
            if (stepsToMove == 0) {
                stepsToMove = 1; // Ensure at least one step is taken
            }
            _stepAccumulator -= stepsToMove;

            if (direction_ > 0) {
                _currentTenths += stepsToMove;
                if (_currentTenths > _targetTenths) {
                    _currentTenths = _targetTenths;
                }
            } else if (direction_ < 0) {
                _currentTenths -= stepsToMove;
                if (_currentTenths < _targetTenths) {
                    _currentTenths = _targetTenths;
                }
            }
            writeTenths(_currentTenths);
        }
    }

    if (_detachAfterMoving && _currentTenths == _targetTenths) {
        _servo.detach();
        _detachAfterMoving = false;
    }
}

void ServoControl::on(){
    if (!_servo.attached()) {
		_servo.attach(_GPIO, SERVO_MIN(), SERVO_MAX());
	}
}

void ServoControl::off() {
    if (isFlag(_flags, SERVO_AUTO_REVERSE)) {
        _targetTenths = _tlimit1;
    }
    _detachAfterMoving = true;
}

void ServoControl::setLimit(int limit1, int limit2) {
    if (limit1 > limit2) {
        _flags = setFlag(_flags, SERVO_REVERSE);
    }
    else {
        _flags = removeFlag(_flags, SERVO_REVERSE);
    }

    _tlimit1 = min(limit1, limit2) * 10;
    _tlimit2 = max(limit1, limit2) * 10;
    setIntervalTime(_travelTime, 10);
}

void ServoControl::setTenths(int tenths) {
    Serial.println("ServoControl::setTenths");
    Serial.print("    Tenths: "); Serial.println(tenths);
    if (tenths < _tlimit1) {
        tenths = _tlimit1;
    }
    else if (tenths > _tlimit2) {
        tenths = _tlimit2;
    }

    _targetTenths = tenths;
    _intervalTimer.start(_intervalTime);

    Serial.print("    Target Tenths: "); Serial.println(_targetTenths);
}

void ServoControl::setPercentPosition(int percentage) {
	Serial.println("ServoControl::setPercentPosition");
	Serial.print("    Percent: "); Serial.println(percentage);
    
    if (percentage < 0)
        percentage = 0;
    else if (percentage > 100)
		percentage = 100;
    uint16_t tenths_;
    if (!isFlag(_flags, SERVO_REVERSE)) {
        tenths_ = map(percentage, 0, 100, _tlimit1, _tlimit2);
    } else {
        tenths_ = map(percentage, 100, 0, _tlimit1, _tlimit2);
    }

    setTenths(tenths_);
}

void ServoControl::setAngle(uint8_t degrees) {
    if (degrees < 0)
        degrees = 0;
    else if (degrees > 180)
		degrees = 180;
    Serial.println("ServoControl::setAngle");
    Serial.print("    Angle: "); Serial.println(degrees);
    
	setTenths(degrees * 10); // Set the target position in tenths
}

uint32_t ServoControl::getTenths() {
    return _currentTenths; // Return current position in tenths
}

uint8_t ServoControl::getAngle() {
	return _currentTenths / 10; // Convert tenths to degrees
}

void ServoControl::writeTenths(int tenths) {
    _servo.writeMicroseconds(map(tenths, 0, 1800, SERVO_MIN(), SERVO_MAX()));
}

// Set the travel time in milliseconds and calculate the interval based on the limits
void ServoControl::setTravelTime(uint16_t ms) {
    _travelTime = ms;
    setIntervalTime(ms, 10);
}

void ServoControl::setIntervalTime(uint16_t interval){
    setIntervalTime(interval, 10);
}

void ServoControl::setIntervalTime(uint16_t travelTime, uint16_t minIntervalTime) {
    if (minIntervalTime < 10) {
        minIntervalTime = 10;
    }

    uint16_t steps_ = abs(_tlimit2 - _tlimit1);
    if (steps_ == 0) steps_ = 1;

    float interval_ = (float)travelTime / (float)steps_;
    if (interval_ < minIntervalTime) interval_ = minIntervalTime;

    _intervalTime = (uint16_t)interval_;
    _intervalSteps = 1; // Immer 1 Schritt pro Intervall
    _stepAccumulator = 0.0f;

    _intervalTimer.start(_intervalTime);

    Serial.println("ServoControl::SetIntervalTime");
    Serial.print("    travelTime: "); Serial.println(travelTime);
    Serial.print("    minIntervalTime: "); Serial.println(minIntervalTime);
    Serial.print("    steps: "); Serial.println(steps_);
    Serial.print("    interval (ms/step): "); Serial.println(_intervalTime);
    Serial.print("    steps per interval: 1 (mit Akkumulation)");
}

ServoBounce::ServoBounce(int8_t ServoPort, int limit1, int limit2, int travelTime, unsigned int flags) :
    ServoControl(ServoPort, limit1, limit2, travelTime, flags),
    _bounceLimit(0),
    _bounceTime(100){
	Serial.println("ServoBounce::ServoBounce");

	_bounceTimer.stop();
}

void ServoBounce::process() {
    ServoControl::process();

    if (_bounceTimer.started()) {
		//Serial.println("ServoBounce::process - Bouncing was started.");

        if (_bounceTimer.repeat()) {
            _bounceCount++;

            if (_bounceDirection) {
                // Move _bounceLimit tenths away from the limit
                writeTenths(_bounceLimit);
            }
            else {
                // Move back to the limit
                writeTenths(_targetTenths);
                
            }
            _bounceDirection = !_bounceDirection; // Toggle direction for next bounce
        }

        if (_bounceCount >= 10) {
            _bounceTimer.stop();
            _bounceStarted = false;
            _bounceCount = 0; 
			writeTenths(_targetTenths);
		}

        return;
    }

    if (isMoving() && !_shouldBounce) {
        _shouldBounce = true;
        if (isClockwise()) {
            _bounceDirection = true; // Clockwise
            _bounceLimit = _targetTenths - 50;
        }
        else {
            _bounceDirection = false; // Counter-clockwise
            _bounceLimit = _targetTenths + 50;
        }

        _bounceAllowed = false;
        if (_bounceDirection && isFlag(_flags, SERVO_BOUNCE_L1)) {
            _bounceAllowed = true;
        }
        else if (!_bounceDirection && isFlag(_flags, SERVO_BOUNCE_L2)) {
            _bounceAllowed = true;
        }
    }

    if (!isMoving() && _shouldBounce && _bounceAllowed) {
        _bounceTimer.start(100);
		_shouldBounce = false;
		_bounceStarted = true;
    }

    if (!_bounceAllowed && _shouldBounce) {
        // If bouncing is not allowed, reset the state
        _shouldBounce = false;
        _bounceStarted = false;
        _bounceCount = 0;
    }
}

void ServoBounce::on(){
    Serial.println("ServoBounce::on");
    ServoControl::on();
}

void ServoBounce::off(){
    Serial.println("ServoBounce::off");
	ServoControl::off();
}
