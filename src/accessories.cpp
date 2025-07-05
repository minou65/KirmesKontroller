#include "accessories.h"

// Class accessories, dir Mutter aller Klassen
accessories::accessories(uint16_t BaseAddress_, byte BaseChannel_) :
	BaseAddress(BaseAddress_),
	BaseChannel(BaseChannel_),
	IsActive(false) {
}

accessories::accessories(uint16_t BaseAddress_, byte BaseChannel_, byte Mode_):
	BaseAddress(BaseAddress_),
	BaseChannel(BaseChannel_),
	Mode(Mode_) {
}

accessories::~accessories() {
};

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

void accessories::notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_){
}

void accessories::notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps){
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
