#include "accessories.h"
#include "SoundControl.h"

// Class accessories, dir Mutter aller Klassen
accessories::accessories(uint16_t BaseAddress_, byte BaseChannel_) :
	BaseAddress(BaseAddress_),
	BaseChannel(BaseChannel_),
	Mode(0),
	IsActive(false),
	_Input(-1),
	_TimeActive(0),
	_sound({ std::string(""), 15, 0, false}) {
}

accessories::accessories(uint16_t BaseAddress_, byte BaseChannel_, byte Mode_):
	accessories(BaseAddress_, BaseChannel_) {
	Mode = Mode_;
}

accessories::~accessories() {
};

void accessories::process() {
	//Serial.print("accessories::process ");
	//Serial.print("    Input pin: "); Serial.print(_Input);
	//Serial.print("    Time active: "); Serial.println(_TimeActive);

	if (_Input > 0 && !isOn() && !digitalRead(_Input)) {
		Serial.println("accessories::process: Input is LOW, turning on");
		on();

	}
	if (_Input > 0 && _TimeActive == 0 && isOn() && digitalRead(_Input)) {
		Serial.println("accessories::process: Input is HIGH, turning off");
		off();
	}
	if (_TimeActive > 0 && isOn() && _timer.done()) {
		Serial.println("accessories::process: Timer done, turning off");
		off();
	}
}

void accessories::on() {
	Serial.println("accessories::on");
	IsActive = true;
	if (_TimeActive > 0) {
		_timer.start(_TimeActive);
		Serial.print("    accessory timer started for "); Serial.print(_TimeActive); Serial.println(" ms");
	}
	if (_sound.filename.length() > 0 && !isplayingSound()) {
		Serial.print("    Playing sound: "); Serial.println(_sound.filename.c_str());
		playSoundLoop(_sound.filename.c_str());
	}
}

void accessories::off() {
	Serial.println("accessories::off");
	IsActive = false;
	if (_TimeActive > 0) {
		_timer.stop();
		Serial.println("    accessory timer stopped");
	}	
	if (_sound.filename.length() > 0 && isplayingSound()) {
		Serial.print("    Stopping sound: "); Serial.println(_sound.filename.c_str());
		stopSound();
	}
}

void accessories::notifyAddress(uint16_t Address_, uint8_t cmd_) {
}

void accessories::notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_){
}

void accessories::notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps){
}

void accessories::setInputPin(uint8_t pin){
	Serial.print("accessories::setInputPin ");
	if (pin != _Input) {
		_Input = pin;
		pinMode(pin, INPUT_PULLUP);
		Serial.print("    Pin set to: "); Serial.println(_Input);
	}
}

uint8_t accessories::getInputPin(){
	return _Input;
}

void accessories::setTimer(int16_t time){
	_TimeActive = time;
}

bool accessories::isOn() const{
	return IsActive;
}

uint16_t accessories::Address(){
	return BaseAddress;
}

byte accessories::Channel(){
	return BaseChannel;
}

byte accessories::ChannelMode(){
	return Mode;
}

// class Signal, die Mutter aller Signale
Signal::Signal(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Mode_, byte Brightness1_, byte Brightness2_) :
	accessories(BaseAddress_, BaseChannel_, Mode_),
	DayLightAddress(DayLightAddress_),
	Brightness1(Brightness1_),
	Brightness2(Brightness2_),
	Fahrbegriff(254),
	NewFahrbegriff(0){

}

bool Signal::SignalState(){
	return State;
}
