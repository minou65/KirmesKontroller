
// #define NOTIFY_DCC_ACCSTATE_MSG
// #define NOTIFY_DCC_MSG
// #define NOTIFY_TURNOUT_MSG
//#define NOTIFY_DCC_CV_WRITE_MSG
//#define NOTIFY_DCC_CV_CHANGE_MSG
//#define DEBUG_MSG

#include <Arduino.h>

#include "neotimer.h"
#include "version.h"
#include "MotorControl.h"
#include "SoundControl.h"
#include "Blinkers.h"
#include "pinmapping.h"
#include "Vector.h"
#include "accessories.h"
#include "webhandling.h"
#include "NMRAhandling.h"

char Version[] = VERSION_STR;

// Liste mit allen Decoderobjekten (Apps)
Vector<accessories*> decoder;

// Task handle (Core 0 on ESP32)
TaskHandle_t TaskHandle;

bool ResetDCCDecoder = false;

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {
#ifdef  NOTIFY_TURNOUT_MSG
	Serial.print("notifyDccAccTurnoutOutput: Turnout: ");
	Serial.print(Addr, DEC);
	Serial.print(" Direction: ");
	Serial.print(Direction ? "Closed" : "Thrown");
	Serial.print(" Output: ");
	Serial.println(OutputPower ? "On" : "Off");
#endif

	for (int _i = 0; _i < decoder.Size(); _i++) {
		decoder[_i]->notifyTurnoutAddress(Addr, Direction, OutputPower);
	}
}

void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps) {
	for (int i = 0; i < decoder.Size(); i++) {

		// is it a Servo?
		if (decoder[i]->ChannelMode() == 230) {
			decoder[i]->notifyDccSpeed(Addr, Speed, ForwardDir, SpeedSteps);
		}
	}
}

void notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State) {
	uint8_t _OutputNum = OutputAddr >> 1;  //shift over the bits so the outputaddr is 0 to 3
	uint8_t _Cmd = OutputAddr & 0b00000001;  //JMRI puts out the state as the right most bit of pDccMsg->Data[1], the state argument doesnt change in JMRI Turnout.

#ifdef NOTIFY_DCC_ACCSTATE_MSG
	Serial.print("AccState - ");
	Serial.print("Raw addr: ");
	Serial.print(Addr);
	Serial.print(" BoardAddr: ");
	Serial.print(BoardAddr, DEC);
	Serial.print(" OutputAddr: ");
	Serial.print(OutputAddr, DEC);
	Serial.print(" Output: ");
	Serial.print(_OutputNum);
	Serial.print(" State: 0b");
	Serial.print(State, BIN);
	Serial.print(" Command: ");
	Serial.print(_Cmd == 1 ? "Closed" : "Thrown");
	Serial.println();
	//Serial.print("    zDecoder: "); Serial.println(BoardAddr + OutputAddr);
#endif 

#ifdef  NOTIFY_TURNOUT_MSG
	Serial.print("zDecoder: Address"); Serial.print(Addr);
	Serial.print(_Cmd == 1 ? " :Closed" : " :Thrown");
	Serial.println();
#endif

	for (int _i = 0; _i < decoder.Size(); _i++) {
		decoder[_i]->notifyAddress(Addr, _Cmd);
	}
}

void notifyDccFunc(uint16_t Addr, uint8_t FuncNum, uint8_t FuncState) {

	if (FuncNum != 1)
		return;

	for (int i = 0; i < decoder.Size(); i++) {

		// is it a Servo?
		if (decoder[i]->ChannelMode() == 230) {
			if (FuncState & 0x10)
				decoder[i]->on();
			else
				decoder[i]->off();
		}
	}

	if (FuncNum != 1)
		return;
}

void handleDecoderGroup(uint8_t DecoderGroup) {
	if (DecoderGroupIsActive(DecoderGroup)) {
		if (decoder[DecoderGroup]->isOn()) {
			decoder[DecoderGroup]->off();
		}
		else {
			decoder[DecoderGroup]->on();
		}
	}
}

bool DecoderGroupIsEnabled(uint8_t DecoderGroup) {
	if (DecoderGroupIsActive(DecoderGroup)) {
		return decoder[DecoderGroup]->isOn();
	}
	else {
		return false;
	}
}

bool DecoderGroupIsActive(uint8_t DecoderGroup) {
	if (DecoderGroup <= decoder.Size() && decoder[DecoderGroup] != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

void kDecoderReset() {
	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		outputgroup_->applyDefaultValues();
		outputgroup_->setActive(false);

		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}

	ServoGroup* servogroup_ = &ServoGroup1;
	while (servogroup_ != nullptr) {
		servogroup_->applyDefaultValues();
		servogroup_->setActive(false);

		servogroup_ = (ServoGroup*)servogroup_->getNext();
	}
}

void kDecoderInit(void) {
	uint8_t channel_ = 0;

	// Decoderobjekte in decoder löschen
	decoder.Erase(0, decoder.Size());
	decoder.Clear();
	Vector<accessories*>().Swap(decoder);

	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		if (channel_ > 15) {
			Serial.println("no more free channels!");
			break;
		}

		if (outputgroup_->isActive()) {
			uint8_t Mode_ = atoi(outputgroup_->_ModeValue);
			uint8_t Count_ = atoi(outputgroup_->_NumberValue);
			uint16_t Address_ = atoi(outputgroup_->_AddressValue);
			uint8_t TimeOn_ = atoi(outputgroup_->_TimeOnValue);
			uint8_t TimeOff_ = atoi(outputgroup_->_TimeOffValue);
			uint8_t Multiplier_ = atoi(outputgroup_->_MultiplierValue); // Multiplikator
			uint8_t TimeOnFade_ = atoi(outputgroup_->_TimeOnFadeValue);
			uint8_t TimeOffFade_ = atoi(outputgroup_->_TimeOffFadeValue);
			uint8_t Brightness_ = atoi(outputgroup_->_BrightnessValue);

			uint16_t DayLightAddress_ = 0;
			uint8_t DayBrightness_ = 255;
			uint8_t NightBrightness_ = 150;

			SoundSettings sound_;
			strncpy(sound_.filename, outputgroup_->getSoundFilename() ? outputgroup_->getSoundFilename() : "", sizeof(sound_.filename) - 1);
			sound_.filename[sizeof(sound_.filename) - 1] = '\0'; // Nullterminierung sicherstellen
			sound_.volume = soundGroup.getVolume();
			sound_.balance = soundGroup.getBalance();
			sound_.mono = soundGroup.isMono();

			Serial.print(F("Values for channel ")); Serial.print(channel_); Serial.println(F(" preserved"));
			Serial.print(F("    Channels used: ")); Serial.println(Count_);
			Serial.print(F("    Address: ")); Serial.println(Address_);

			// Einrichten des Ports
			accessories* newAccessory = nullptr;
			switch (Mode_) {
			case 0:
				newAccessory = new accessories(Address_, channel_);
				channel_ += Count_;
				break;
			case 40: // einfacher Ausgang
				newAccessory = new Ausgang(Address_, channel_);
				channel_ += 1;
				break;
			case 50: // Blinker
				newAccessory = new Blinker(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_, TimeOnFade_, TimeOffFade_, Mode_);
				channel_ += 1;
				break;
			case 51: // Wechselblinker
				newAccessory = new Wechselblinker(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_, TimeOnFade_, TimeOffFade_);
				channel_ += 2;
				break;
			case 80: // TV
				newAccessory = new Fernseher(Address_, channel_);
				channel_ += 1;
				break;
			case 81: // Schweisslicht
				newAccessory = new Schweissen(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_);
				channel_ += 3;
				break;
			case 82: // Feuer
				newAccessory = new Feuer(Address_, channel_);
				channel_ += 3;
				break;
			case 83: // Blitzlicht
				newAccessory = new Blitzlicht(Address_, channel_, TimeOnFade_ * 1000, TimeOffFade_ * 1000, TimeOn_ * Multiplier_, TimeOff_ * Multiplier_);
				channel_ += 1;
				break;
			case 203:// Motor
				newAccessory = new Motor(Address_, channel_, Mode_, TimeOn_ * Multiplier_); // Create a new Motor accessory
				channel_ += 1;
				break;
			}

			if (newAccessory != nullptr) {
				Serial.print(F("    Mode: ")); Serial.println(Mode_);

				Accessory* accessory = static_cast<Accessory*>(newAccessory);
				accessory->setSoundSettings(sound_);
				accessory->setInputPin(outputgroup_->getInputPin());
				accessory->setTimer(outputgroup_->getActiveDuration());

				// Check the type of newAccessory
				if (newAccessory->getType() == AccessoryType::LED) {
					Ausgang* ledAccessory = static_cast<Ausgang*>(newAccessory);
					// If the cast is successful, set the brightness
					ledAccessory->SetMaxBrightness(Brightness_);
				}
				decoder.PushBack(newAccessory);
			}
		}
		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}

	ServoGroup* servogroup_ = &ServoGroup1;
	while (servogroup_ != nullptr) {
		if (channel_ > 15) {
			Serial.println("no more free channels!");
			break;
		}

		if (servogroup_->isActive()) {

		}
		servogroup_ = (ServoGroup*)servogroup_->getNext();
	}
}

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}

	Serial.println("Starting with Firmware " + String(VERSION));

	for (uint8_t _i = 0; _i < sizeof(ChannelToGPIOMapping) - 1; _i++) {
		pinMode(ChannelToGPIOMapping[_i], OUTPUT);
		digitalWrite(ChannelToGPIOMapping[_i], LOW);
	}


	setupWeb(); // Set up web handling
	setupSound(); // Set up sound system
	NMRAsetup();

	kDecoderInit(); // Initialize the decoder

	if (CONFIG_PIN >= 0) {
		pinMode(CONFIG_PIN, INPUT);
		if (digitalRead(CONFIG_PIN) == LOW) {
			Serial.println("config pin was pressed, reset decoder");
			kDecoderReset();
		}
	}

	xTaskCreatePinnedToCore(
		loop2, /* Function to implement the task */
		"TaskHandle", /* Name of the task */
		10000,  /* Stack size in words */
		NULL,  /* Task input parameter */
		0,  /* Priority of the task */
		&TaskHandle,  /* Task handle. */
		0 /* Core where the task should run */
	);

	Serial.println("Everything has been initialized");
}

void loop() {
	NMRAloop(); // Handle NMRA DCC messages
	loopWeb(); // Handle web requests
	loopSound(); // Handle sound playback
}

void loop2(void* parameter) {

	for (;;) {   // Endless loop
		if (ResetDCCDecoder) {
			kDecoderInit();
			ResetDCCDecoder = false;
		}

		for (int _i = 0; _i < decoder.Size(); _i++) {
			decoder[_i]->process();
		}
	}
}
