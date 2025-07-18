// accessories.h

#ifndef _ACCESSORIES_h
#define _ACCESSORIES_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "neotimer.h"

enum class AccessoryType {
	None,
	LED,
	Servo,
	Coil,
	Output,
	Signal,
	Watchdog,
	Motor,
	ServoImpulse,
	ServoFlip,
	ServoPendel,
};

struct SoundSettings {
	char filename[61];
	uint16_t volume;
	int8_t balance;
	bool mono;
};

class accessories {
	protected:
	uint16_t BaseAddress;
	byte BaseChannel;
	bool IsActive;
	
	byte Mode;

public:
	accessories() = default;
	accessories(uint16_t baseAddress, byte baseChannel);
	accessories(uint16_t baseAddress, byte baseChannel, byte mode);
	~accessories();

	virtual void process();
	virtual void on();
	virtual void off();

	virtual AccessoryType getType() const;

	virtual void notifyAddress(uint16_t Address_, uint8_t cmd_);
	virtual void notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_);
	virtual void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps);

	bool isOn() const;
	uint16_t Address();
	byte Channel();

	byte ChannelMode();
};

// Accessory class, the base class for all output accessories
class Accessory : public accessories {
protected:
	SoundSettings _sound;
	uint8_t _Input;
	uint16_t _TimeActive;
	Neotimer _timer = Neotimer();


public:
	Accessory() = default;
	Accessory(uint16_t BaseAddress_, byte BaseChannel_);
	Accessory(uint16_t BaseAddress, byte BaseChannel_y, byte Mode_);
	~Accessory();

	void process() override;
	void on() override;
	void off() override;

	void setSoundSettings(SoundSettings sound) {
		_sound = sound;
	}
	SoundSettings getSoundSettings() {
		return _sound;
	}
	void setInputPin(uint8_t pin);
	uint8_t getInputPin();

	void setTimer(uint16_t time);
};

// The base of all signals
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

