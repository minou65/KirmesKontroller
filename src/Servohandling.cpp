// 
// 
// 

#include "Servohandling.h"
#include "pinmapping.h"
#include "common.h"


// ServoAccessory is a class that represents a servo motor accessory.
// It inherits from the Accessory class and provides functionality to control a servo motor.
// It includes methods to initialize the servo, process its state, and control its position.
ServoAccessory::ServoAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime) :
	Accessory(Address, ServoPort, 0),
	_ServoPort(ServoPort),
	_limit1(limit1),
	_limit2(limit2),
	_travelTime(travelTime) {
	Serial.println(F("ServoAccessory::ServoAccessory"));
	_servoControl = new ServoControl(ServoPort, limit1, limit2, travelTime);
	off();
}

ServoAccessory::~ServoAccessory() {
	Accessory::~Accessory();
	if (_servoControl) {
		_servoControl->~ServoControl();
		_servoControl = nullptr;
	}
}

void ServoAccessory::process() {
	Accessory::process();
	if (_servoControl) {
		_servoControl->process();
	}
}

void ServoAccessory::on() {
	Accessory::on();
	MoveServo(100); // Move to maximum position
}

void ServoAccessory::off() {
	Accessory::off();
	MoveServo(0); // Move to minimum position
}

void ServoAccessory::MoveServo(uint16_t percentage) {
	Serial.println(F("ServoAccessory::MoveServo()"));
	Serial.print(F("    Percentage: ")); Serial.println(percentage);
	if (_servoControl == nullptr) {
		Serial.println(F("ServoControl is not initialized!"));
		return;
	}

	_servoControl->setPercentPosition(percentage);
}

// ServoImpulseAccessory is a class that represents a servo motor accessory with an impulse feature.
// It inherits from the ServoAccessory class and adds functionality to control the servo for a specific impulse duration.
// after a certain time, the servo will automatically turn off.
ServoImpulseAccessory::ServoImpulseAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime, uint16_t impulsTime) :
	ServoAccessory(Address, ServoPort, limit1, limit2, travelTime),
	_impulsTime(impulsTime) {

	if (_impulsTime < travelTime) {
		_impulsTime = travelTime; // Ensure impuls time is at least as long as travel time
	}
}

ServoImpulseAccessory::~ServoImpulseAccessory() {
	ServoAccessory::~ServoAccessory();
}

void ServoImpulseAccessory::process() {
	ServoAccessory::process();
	if (_timer.done() && isOn()) {
		off();
	}
}

void ServoImpulseAccessory::on() {
	Serial.println(F("ServoImpulseAccessory::on()"));
	ServoAccessory::on();
	_timer.start(_impulsTime); // Start the timer for the impulse duration
}

void ServoImpulseAccessory::off() {
	Serial.println(F("ServoImpulseAccessory::off()"));
	ServoAccessory::off();
	_timer.stop();

}

// ServoFlipAccessory is a class that represents a servo motor accessory with a flipping feature.
// It inherits from the ServoAccessory class and provides functionality to control the servo for flipping actions.
// on on the servo will move to one limit, and on off it will move to the other limit.
ServoFlipAccessory::ServoFlipAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime) :
	ServoAccessory(Address, ServoPort, limit1, limit2, travelTime) {
	Serial.println(F("ServoFlipAccessory::ServoFlipAccessory()"));
}

ServoFlipAccessory::~ServoFlipAccessory() {
	Serial.println(F("ServoFlipAccessory::~ServoFlipAccessory()"));
	ServoAccessory::~ServoAccessory();
}

void ServoFlipAccessory::process() {
	ServoAccessory::process();
}

void ServoFlipAccessory::on() {
	Serial.println(F("ServoFlipAccessory::on()"));
	ServoAccessory::on();
}

void ServoFlipAccessory::off() {
	Serial.println(F("ServoFlipAccessory::off()"));
	ServoAccessory::off();
}

ServoPendelAccessory::ServoPendelAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime, uint16_t pendelTime) :
	ServoAccessory(Address, ServoPort, limit1, limit2, travelTime),
	_onTime(pendelTime)
{
	Serial.println(F("ServoPendelAccessory::ServoPendelAccessory()"));
	if (_onTime < travelTime) {
		_onTime = travelTime; // Ensure pendulum time is at least as long as travel time
	}
	_offTime = _onTime; // Set off time to the same value initially
	_direction = true;
	

}

ServoPendelAccessory::ServoPendelAccessory(uint16_t Address, int8_t ServoPort, int16_t limit1, int16_t limit2, int16_t travelTime, uint16_t onTime, uint16_t offTime) :
	ServoAccessory(Address, ServoPort, limit1, limit2, travelTime),
	_onTime(onTime),
	_offTime(offTime)
{
	Serial.println(F("ServoPendelAccessory::ServoPendelAccessory()"));

	if (_onTime < travelTime) {
		_onTime = travelTime; // Ensure on time is at least as long as travel time
	}
	if (_offTime < travelTime) {
		_offTime = travelTime; // Ensure off time is at least as long as travel time
	}
	_direction = true;

	Serial.print(F("    onTime: ")); Serial.println(_onTime);
	Serial.print(F("    offTime: ")); Serial.println(_offTime);

}


ServoPendelAccessory::~ServoPendelAccessory() {
	Serial.println(F("ServoPendelAccessory::~ServoPendelAccessory()"));
	ServoAccessory::~ServoAccessory();
}

void ServoPendelAccessory::process() {
	ServoAccessory::process();

	if (isOn()) {
		if (_timerPendel.done() || !_started) {
			_started = true; // Mark that the pendulum has started moving

			// Set position: 0% for one direction, 100% for the other
			if (_direction) {
				MoveServo(100); // Move to _limit2
				_timerPendel.start(_onTime); // Start with _onTime
			}
			else {
				MoveServo(0);  // Move to _limit1
				_timerPendel.start(_offTime); // Start with _offTime
			}
			_direction = !_direction;
		}
	}
}

void ServoPendelAccessory::on() {
	Serial.println(F("ServoPendelAccessory::on()"));
	IsActive = true; // Set the accessory to active state
	_timerPendel.start(_onTime); // Start the timer for pendulum movement
	_started = false;
}

void ServoPendelAccessory::off() {
	Serial.println(F("ServoPendelAccessory::off()"));
	IsActive = false; // Set the accessory to inactive state
	_timerPendel.stop(); // Stop the timer when turned off

}

