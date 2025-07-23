#include "ServoControl.h"
#include "pinmapping.h"

ServoControl::ServoControl(int8_t ServoPort, int limit1_, int limit2_, int travelTime_, unsigned int flags_) :
    _GPIO(ServoPort),
    _limit1(limit1_),
    _limit2(limit2_),
    _tlimit1(limit1_ * 10),
    _tlimit2(limit2_ * 10),
    _travelTime(travelTime_),
    _IsActive(false),
    _flags(flags_) {


    SetIntervalTime(_travelTime, 10);

    _currentTenths = _tlimit1;
    _targetThenths = _currentTenths; // Initialize target position to current position

    _servo.setPeriodHertz(50);
    _servo.attach(_GPIO, SERVO_MIN(), SERVO_MAX());
    _servo.write(_currentTenths / 10);


    Serial.println("ServoControl::ServoControl");

    Serial.print("    Limit1:     "); Serial.println(_limit1);
    Serial.print("    Limit2:     "); Serial.println(_limit2);
    Serial.print("    tLimit1:    "); Serial.println(_tlimit1);
    Serial.print("    tLimit2:    "); Serial.println(_tlimit2);

}

ServoControl::~ServoControl() {
	_servo.detach();
    _servo.~Servo();
    _intervalTimer.~Neotimer();
}

void ServoControl::process() {

	int16_t direction_;

    if (_currentTenths != _targetThenths) {
		direction_ = _targetThenths - _currentTenths;

        if (direction_ > 0) {
            if (_intervalSteps == 0) {
                _currentTenths = _targetThenths;
            } else if (_intervalTimer.repeat()) {
			    _currentTenths += _intervalSteps;
                if (_currentTenths > _targetThenths) {
                    _currentTenths = _targetThenths;
			    }
            }

        }
        else if (direction_ < 0) {
            if (_intervalSteps == 0) {
                _currentTenths = _targetThenths;
            }
            else if (_intervalTimer.repeat()) {
                _currentTenths -= _intervalSteps;
                if (_currentTenths < _targetThenths) {
                    _currentTenths = _targetThenths;
                }
            }
        }
        _servo.write(_currentTenths / 10); // Convert tenths to degrees
    }

}

void ServoControl::on(){

}

void ServoControl::off(){

}

void ServoControl::setStartAngle(uint8_t degrees){
    if (degrees < 0)
        degrees = 0;
    else if (degrees > 180)
        degrees = 180;
    _limit1 = degrees;
    _tlimit1 = degrees * 10;
	SetIntervalTime(_travelTime, 10);
}

void ServoControl::setEndAngle(uint8_t degrees){
    if (degrees < 0)
        degrees = 0;
    else if (degrees > 180)
        degrees = 180;
    _limit2 = degrees;
    _tlimit2 = degrees * 10;
    SetIntervalTime(_travelTime, 10);
}

void ServoControl::setThenths(int tenths) {
    Serial.println("ServoControl::setThenths");
    Serial.print("    Tenths: "); Serial.println(tenths);
    if (tenths < std::min(_tlimit1, _tlimit2)) {
        tenths = std::min(_tlimit1, _tlimit2);
    }
    else if (tenths > std::max(_tlimit1, _tlimit2)) {
        tenths = std::max(_tlimit1, _tlimit2);
    }

    _targetThenths = tenths;
    _intervalTimer.start(_intervalTime);

    Serial.print("    Target Tenths: "); Serial.println(_targetThenths);
}

void ServoControl::setPosition(int percentage) {
	Serial.println("ServoControl::setPosition");
	Serial.print("    Percent: "); Serial.println(percentage);
    
    if (percentage < 0)
        percentage = 0;
    else if (percentage > 100)
		percentage = 100;

    uint32_t range_ = std::max(_tlimit1, _tlimit2) - std::min(_tlimit1, _tlimit2);
    uint32_t tenths_ = (percentage * range_) / 100;

    setThenths(tenths_);
}

void ServoControl::setAngle(uint8_t degrees) {
    if (degrees < 0)
        degrees = 0;
    else if (degrees > 180)
		degrees = 180;
    Serial.println("ServoControl::setAngle");
    Serial.print("    Angle: "); Serial.println(degrees);
    
	setThenths(degrees * 10); // Set the target position in tenths
}

uint32_t ServoControl::getTenths() {
    return _currentTenths; // Return current position in tenths
}

uint8_t ServoControl::getAngle() {
	return _currentTenths / 10; // Convert tenths to degrees
}

void ServoControl::writeTenths(int tenths) {

    // Begrenzung auf die aktuellen Limits
    int minTenths = std::min(_tlimit1, _tlimit2);
    int maxTenths = std::max(_tlimit1, _tlimit2);
    tenths = std::max(minTenths, std::min(tenths, maxTenths));

    Serial.println("ServoControl::writeTenths");
    Serial.print("    tenths: "); Serial.println(tenths);

    _servo.writeMicroseconds(map(
        tenths,
        minTenths, maxTenths,
        SERVO_MIN(), SERVO_MAX()
    ));
}

// Set the travel time in milliseconds and calculate the interval based on the limits
void ServoControl::setTravelTime(uint16_t ms) {
    _travelTime = ms;
    SetIntervalTime(ms, 10);
}

void ServoControl::SetIntervalTime(uint16_t interval){
    SetIntervalTime(interval, 10);
}

void ServoControl::SetIntervalTime(uint16_t travelTime, uint16_t minIntervalTime) {
    if (minIntervalTime < 10) {
        minIntervalTime = 10;
    }

    uint16_t steps_ = abs(_tlimit2 - _tlimit1);
    if (steps_ == 0) steps_ = 1;

    uint16_t interval_ = travelTime / steps_;
    if (interval_ < minIntervalTime) interval_ = minIntervalTime;

    // Calculate how many steps to move per interval
    uint16_t stepsPerInterval_ = (steps_ * interval_) / travelTime;
    if (stepsPerInterval_ == 0) stepsPerInterval_ = 1;
    _intervalSteps = stepsPerInterval_;
	_intervalTime = interval_;

    _intervalTimer.start(interval_);

    Serial.println("ServoControl::SetIntervalTime");
    Serial.print("    travelTime: "); Serial.println(travelTime);
    Serial.print("    minIntervalTime: "); Serial.println(minIntervalTime);
    Serial.print("    steps: "); Serial.println(steps_);
    Serial.print("    interval (ms/step): "); Serial.println(interval_);
    Serial.print("    steps per interval: "); Serial.println(stepsPerInterval_);
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
                // Move 10 tenths away from the limit
                //_servo.write((_bounceLimit - 20) / 10);
                Serial.print("    Moving: "); Serial.println(_bounceLimit);
            }
            else {
                // Move back to the limit
                //_servo.write(_bounceLimit / 10);
                Serial.print("    Moving: "); Serial.println(_targetThenths);
                
            }
            _bounceDirection = !_bounceDirection; // Toggle direction for next bounce
        }

        if (_bounceCount >= 10) {
            _bounceTimer.stop();
            _bounceStarted = false;
            _bounceCount = 0; 
		}

        return;
    }

    if (isMoving() && !_shouldBounce) {
        _shouldBounce = true;
        if (isClockwise()) {
			_bounceDirection = true; // Clockwise
            _bounceLimit = _targetThenths - 50;
        }
        else {
			_bounceDirection = false; // Counter-clockwise
			_bounceLimit = _targetThenths + 50;
        }
		Serial.println("Bounce direction set to: " + String(_bounceDirection ? "clockwise" : "conter clockwise"));
    }

    if (!isMoving() && _shouldBounce) {
        _bounceTimer.start(100);
		_shouldBounce = false;
		_bounceStarted = true;
    }


    return;


 //   if (_startedBouncing && (_bounceCount < 10)) {
 //       //Serial.println("ServoBounce::process - Stopping bounce after 10 iterations.");
 //       if (_bounceTimer.started()) {
	//		Serial.println("ServoBounce::process - Bounce timer is running.");
 //       }

 //       if (_bounceTimer.repeat(5)) {
 //           Serial.println("ServoBounce::process - Bouncing...");
 //           _bounceCount++;
	//	}

	//	//if (_bounceTimer.repeat()) {
	//	//	Serial.println("ServoBounce::process - Bouncing...");
 // //          if (!_bounceDirection) {
 // //              // Move 10 tenths away from the limit
 // //              //_servo.write((_bounceLimit - 20) / 10);
	//	//		Serial.print("    Moving away from limit: "); Serial.println(_bounceLimit - 20);
 // //          }
 // //          else {
 // //              // Move back to the limit
 // //              //_servo.write(_bounceLimit / 10);
	//	//		Serial.print("    Moving to limit: "); Serial.println(_bounceLimit);
 // //          }
 // //          _bounceDirection = !_bounceDirection; // Toggle direction for next bounce
 // //          _bounceCount++;
 // //      }
	//	
 //       return;
 //   } else if (_startedBouncing){
 //       _bounceTimer.stop();
 //       _startedBouncing = false;
	//	_bounceCount = 0; // Reset bounce count
	//	Serial.println("ServoBounce::process - Bounce timer stopped.");
 //       //Serial.println("ServoBounce::process - Bounce timer stopped.");
	//}

    // Normal logic
    if (!_shouldBounce && isMoving()) {
        Serial.println("ServoBounce::process - Servo is moving.");
        _shouldBounce = true;
        _bounceDirection = !isClockwise();
    }

    if (_shouldBounce && !isMoving()) {
        Serial.println("ServoBounce::process - Servo is not moving, starting bounce.");
        if (_bounceDirection) {
            _bounceLimit = _tlimit1;
        }
        else {
            _bounceLimit = _tlimit2;
        }

        // Start the bounce timer
		Serial.println("ServoBounce::process - Starting bounce timer");
        _bounceTimer.start(500);

		_shouldBounce = false;
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
