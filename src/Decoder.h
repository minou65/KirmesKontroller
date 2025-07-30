// Decoder.h

#ifndef _DECODER_h
#define _DECODER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Vector.h"
#include "accessories.h"

class Decoder {
public:
	Decoder() = default;
	~Decoder();

	void add(Accessory* item = nullptr);
	void remove(uint8_t index);
	void reset();

	void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower);
	void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps);
	void notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State);
	void notifyDccFunc(uint16_t Addr, uint8_t FuncNum, uint8_t FuncState);

	void process();
	void on(uint8_t index);
	void off(uint8_t index);
	void toggling(uint8_t index);

	bool isOn(uint8_t index);

	AccessoryType getAccessoryType(uint8_t index);
	uint16_t getAddress(uint8_t index);
	uint8_t getMode(uint8_t index);

private:
	Vector<Accessory*> _list;

	bool isValidObject(uint8_t index);

};


#endif

