#include "accessories.h"

#if SOUND_FEATURE == 1
#include "SoundControl.h"
#endif

accessories::accessories(uint16_t baseAddress, byte baseChannel) :
	BaseAddress(BaseAddress),
	BaseChannel(BaseChannel),
	Mode(0),
	IsActive(false) {

}

accessories::accessories(uint16_t baseAddress, byte baseChannel, byte mode) :
	accessories(BaseAddress, BaseChannel) {

	Mode = Mode;
}

accessories::~accessories(){
}

AccessoryType accessories::getType() const {
	Serial.print("accessories::getType");
	return AccessoryType::None;
}

void accessories::process() {
}

void accessories::on() {
	IsActive = true;
}

void accessories::off() {
	IsActive = false;
}

void accessories::notifyAddress(uint16_t Address_, uint8_t cmd_) {
}

void accessories::notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_) {
}

void accessories::notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps) {
}

bool accessories::isOn() const {
	return IsActive;
}

uint16_t accessories::Address() {
	return BaseAddress;
}

byte accessories::Channel() {
	return BaseChannel;
}

byte accessories::ChannelMode() {
	return Mode;
}

// Class Accessory, die Mutter aller Klassen
#if SOUND_FEATURE == 1 && INPUT_PIN_FEATURE == 1
Accessory::Accessory(uint16_t BaseAddress_, byte BaseChannel_) :
	accessories(BaseAddress_, BaseChannel_),
	_Input(-1),
	_TimeActive(0),
	_sound("", 15, 0, false){
}
#elif INPUT_PIN_FEATURE == 1
Accessory::Accessory(uint16_t BaseAddress_, byte BaseChannel_):
	accessories(BaseAddress_, BaseChannel_),
	_Input(-1),
	_TimeActive(0) {
}
#else
Accessory::Accessory(uint16_t BaseAddress_, byte BaseChannel_):
	accessories(BaseAddress_, BaseChannel_) {
}
#endif

Accessory::Accessory(uint16_t BaseAddress_, byte BaseChannel_, byte Mode_):
	Accessory(BaseAddress_, BaseChannel_) {
	Mode = Mode_;
}

Accessory::~Accessory() {
};

void Accessory::process() {
	//Serial.print("accessories::process ");
	//Serial.print("    Input pin: "); Serial.print(_Input);
	//Serial.print("    Time active: "); Serial.println(_TimeActive);
	accessories::process();

#if INPUT_PIN_FEATURE == 1

	if (_Input > 0 && !isOn() && (digitalRead(_Input) == LOW)) {
		Serial.println("Accessory::process: Input is LOW, turning on");
		on();
		return;
	}
	if (_Input > 0 && isOn() && (digitalRead(_Input) == LOW)) {
		Serial.println("Accessory::process: Input is LOW, reset timer");
		if (_TimeActive > 0) {
			_timer.start(_TimeActive);
			//Serial.print("    accessory timer restarted for "); Serial.print(_TimeActive); Serial.println(" ms");
		}
		return;
	}
	if (_Input > 0 && _TimeActive == 0 && isOn() && (digitalRead(_Input) == HIGH)) {
		Serial.println("Accessory::process: Input is HIGH, turning off");
		off();
		return;
	}
	if (_TimeActive > 0 && isOn() && _timer.done() && (digitalRead(_Input) == HIGH)) {
		Serial.println("Accessory::process: Timer done, turning off");
		off();
		return;
	}
#endif
}

void Accessory::on() {
	Serial.println("Accessory::on");
	accessories::on();

#if INPUT_PIN_FEATURE == 1
	if (_TimeActive > 0) {
		_timer.start(_TimeActive);
		Serial.print("    accessory timer started for "); Serial.print(_TimeActive); Serial.println(" ms");
	}
#endif

#if SOUND_FEATURE == 1
	if (_sound.filename[0] != '\0' && !isplayingSound()) {
		Serial.print("    Playing sound: "); Serial.println(_sound.filename);
		setSoundVolume(_sound.volume);
		setSoundBalance(_sound.balance);
		setSoundMono(_sound.mono);
		playSoundLoop(_sound.filename);
	}
#endif
}

void Accessory::off() {
	Serial.println("Accessory::off");
	accessories::off();

#if INPUT_PIN_FEATURE == 1
	if (_TimeActive > 0) {
		_timer.stop();
		Serial.println("    accessory timer stopped");
	}
#endif

#if SOUND_FEATURE == 1
	if (_sound.filename[0] != '\0' && isplayingSound()) {
		Serial.print("    Stopping sound: "); Serial.println(_sound.filename);
		stopSound();
	}
#endif

}

#if INPUT_PIN_FEATURE == 1
void Accessory::setInputPin(uint8_t pin){
	Serial.print("Accessory::setInputPin ");
	if (pin != _Input) {
		_Input = pin;
		pinMode(pin, INPUT_PULLUP);
		Serial.print("    Pin set to: "); Serial.println(_Input);
	}
}

uint8_t Accessory::getInputPin(){
	return _Input;
}

void Accessory::setTimer(uint16_t time){
	Serial.print("Accessory::setTimer ");
	Serial.print("	Time set to: "); Serial.print(time); Serial.println("ms");
	_TimeActive = time;
}
#endif // INPUT_PIN_FEATURE

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

