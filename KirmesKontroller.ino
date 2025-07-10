#include <Arduino.h>

#include "src/neotimer.h"
#include "src/version.h"
#include "src/MotorControl.h"
#include "src/SoundControl.h"
#include "src/pinmapping.h"
#include "src/Vector.h"
#include "src/accessories.h"


Neotimer InputTimer1(30000);
Neotimer InputTimer2(30000);
Sound sound;



// Liste mit allen Decoderobjekten (Apps)
Vector<accessories*> decoder;

void kDecoderInit(void) {
	uint8_t channel_ = 0;

	// Decoderobjekte in decoder löschen
	decoder.Erase(0, decoder.Size());
	decoder.Clear();
	Vector<accessories*>().Swap(decoder);

	// Initialize the decoder here if needed
	
	sound.begin();
	accessories* newAccessory = nullptr;

	uint8_t Mode_ = 203; // Default Mode for Motor accessory
	uint16_t Address_ = 0;

	newAccessory = new Motor(Address_, Mode_); // Create a new Motor accessory

	if (newAccessory != nullptr) {
		Serial.print(F("    Mode: ")); Serial.println("Motor");
		decoder.PushBack(newAccessory);
	}

	Serial.println("kDecoder Initialized");
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

	kDecoderInit(); // Initialize the decoder

}

void loop() {
	sound.process(); // Process audio tasks

	for (int i_ = 0; i_ < decoder.Size(); i_++) {
		decoder[i_]->process();
	}

	if (digitalRead(INPUT1) == LOW && !sound.isPlaying()) {

		sound.play("/File1.mp3"); // Sound abspielen

		for (int i_ = 0; i_ < decoder.Size(); ++i_) {
			if (decoder[i_]->getType() == AccessoryType::Motor) {
				Motor* motor_ = (Motor*)decoder[i_];
				motor_->on();
			}
		}

		InputTimer1.start();
	}

	if (InputTimer1.done()) {
		for (int i_ = 0; i_ < decoder.Size(); ++i_) {
			if (decoder[i_]->getType() == AccessoryType::Motor) {
				Motor* motor_ = (Motor*)decoder[i_];
				motor_->off();
			}
		}
		sound.stop(); // Sound stoppen
		InputTimer1.stop();
	}
	
	vTaskDelay(1);
}
