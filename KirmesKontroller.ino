#include <Arduino.h>

#include "src/neotimer.h"
#include "src/version.h"
#include "src/MotorControl.h"
#include "src/SoundControl.h"
#include "src/Blinkers.h"
#include "src/pinmapping.h"
#include "src/Vector.h"
#include "src/accessories.h"
#include "src/webhandling.h"

char Version[] = VERSION_STR;


Neotimer InputTimer1(30000);
Neotimer InputTimer2(30000);


// Liste mit allen Decoderobjekten (Apps)
Vector<accessories*> decoder;

bool ResetDCCDecoder = false;

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
	if (DecoderGroup < decoder.Size() && decoder[DecoderGroup] != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

void kDecoderReset() {
	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		outputgroup_->DesignationParam.applyDefaultValue();
		outputgroup_->ModeParam.applyDefaultValue();
		outputgroup_->NumberParam.applyDefaultValue();
		outputgroup_->AddressParam.applyDefaultValue();
		outputgroup_->TimeOnParam.applyDefaultValue();
		outputgroup_->TimeOffParam.applyDefaultValue();
		outputgroup_->TimeOnFadeParam.applyDefaultValue();
		outputgroup_->TimeOffFadeParam.applyDefaultValue();
		outputgroup_->setActive(false);

		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}

	ServoGroup* servogroup_ = &ServoGroup1;
	while (servogroup_ != nullptr) {
		servogroup_->DesignationParam.applyDefaultValue();
		servogroup_->AddressParam.applyDefaultValue();
		servogroup_->TravelTimeParam.applyDefaultValue();
		servogroup_->Limit1Param.applyDefaultValue();
		servogroup_->Limit2Param.applyDefaultValue();
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
			sound_.filename = outputgroup_->getSoundFilename();
			sound_.volume = soundGroup.getVolume();
			sound_.balance = soundGroup.getBalance();
			sound_.mono = soundGroup.isMono();
			sound_.maxPlayTime = outputgroup_->getActiveDuration();

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
				newAccessory = new Motor(Address_, Mode_); // Create a new Motor accessory
				break;
			}

			if (newAccessory != nullptr) {
				Serial.print(F("    Mode: ")); Serial.println(Mode_);

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

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	pinMode(INPUT1, INPUT_PULLUP);
	pinMode(INPUT2, INPUT_PULLUP);

	setupWeb(); // Set up web handling

	setupSound(); // Set up sound system

	kDecoderInit(); // Initialize the decoder

}

void loop() {
	loopWeb(); // Handle web requests
	loopSound(); // Handle sound playback

	for (int i_ = 0; i_ < decoder.Size(); i_++) {
		decoder[i_]->process();
		SoundSettings sound_ = decoder[i_]->getSoundSettings();
		if (!sound_.filename.empty()) {
			if (!decoder[i_]->isOn()) {
				// Der Dateiname ist nicht leer, also kann abgespielt werden
				setSoundVolume(sound_.volume);
				setSoundBalance(sound_.balance);
				playSoundLoop(sound_.filename);
			}
			else {
				stopSound();
			}
		}
	}

	
	
}
