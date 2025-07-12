// accessories.h

#ifndef _ACCESSORIES_h
#define _ACCESSORIES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <string>

enum class AccessoryType {
	None,
	LED,
	Servo,
	Coil,
	Output,
	Signal,
	Watchdog,
	Motor
};

struct SoundSettings {
	std::string filename;
	int volume;
	int balance;
	bool mono;
	int maxPlayTime;
};

// Root Klasse
class accessories {
protected:
	uint16_t BaseAddress;
	byte BaseChannel;

	bool IsActive;
	
	byte Mode;

	SoundSettings _sound;

public:
	accessories() = default;
	accessories(uint16_t BaseAddress_, byte BaseChannel_);
	accessories(uint16_t BaseAddress, byte BaseChannel_y, byte Mode_);
	~accessories();

	virtual void process();
	virtual void on();
	virtual void off();
	virtual AccessoryType getType() const { 
		Serial.print("accessories::getType");
		return AccessoryType::None;
	}

	virtual void notifyAddress(uint16_t Address_, uint8_t cmd_);
	virtual void notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_);
	virtual void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps);

	void setSoundSettings(SoundSettings sound) {
		_sound = sound;
	}
	SoundSettings getSoundSettings() {
		return _sound;
	}

	bool isOn() const;
	uint16_t Address();
	byte Channel();

	byte ChannelMode();

};

// Die Mutter aller Signale
class Signal : public accessories {
protected:
	uint16_t DayLightAddress;
	byte Fahrbegriff;
	byte NewFahrbegriff;

	byte Brightness1;				// Helligkeit Tag
	byte Brightness2;			    // Helligkeit Nacht

	bool State;

public:

	Signal(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Mode_, byte Brightness1_ = 255, byte Brightness2_ = 100);
	bool SignalState();
	AccessoryType getType() const override { return AccessoryType::Signal; }
};

#endif

