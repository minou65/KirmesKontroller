
#include "SoundControl.h"
#include "pinmapping.h"
#include <Arduino.h>

#include "src/neotimer.h"
#include "src/version.h"
#include "src/MotorControl.h"




MotorControl motor1(8, 0, 5000, 8); // Pin 8, PWM Channel 0, Frequency 5000Hz, Resolution 8 bits

Neotimer playTimer1(30000);

bool isPlaying1 = false;

// Callback-Funktion für das Ende der Datei
void audio_eof_mp3(const char* info) {
	Serial.print("eof: ");
	Serial.println(info);
	// Datei erneut abspielen (Endlosschleife)
	if (isPlaying1) {
		audio.connecttoFS(SD_MMC, FILE_ON_SD); // Endlosschleife, solange Timer läuft
	}
}

void audio_info(const char* info) {
	Serial.print("info: ");
	Serial.println(info);
}
void audio_id3data(const char* info) {
	Serial.print("id3: ");
	Serial.println(info);
}

void audio_left(void* parameter) {
	// Callback-Funktion für den linken Audiokanal
	Serial.println("Left channel callback");

	audio.setVolume(8); // 0...21(max)
	audio.setBalance(-16); // -16...16 (left to right)

	if (!audio.connecttoFS(SD_MMC, FILE_ON_SD)) {
		Serial.println("Failed to play file");
	}

	//vTaskDelete(NULL); // task finishes when all files were played
}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	pinMode(Input1, INPUT_PULLUP);
	pinMode(Input2, INPUT_PULLUP);

	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}
	SD_MMC.setPins(SPI_SCK, SPI_MOSI, SPI_MISO);
	if (!SD_MMC.begin("/sdcard", true)) {
		Serial.println("Card Mount Failed");
		return;
	}

	audio.setPinout(I2S_BCLK, I2S_LRCLK, I2S_DOUT);

}

void loop() {
	audio.loop();
	if (digitalRead(Input1) == LOW && !isPlaying1) {
		Serial.println("Input1 pressed");
		isPlaying1 = true;
		playTimer1.start();
		audio_left(NULL); // Start the left channel audio task
		motor1.startMotor(); // Start the motor

		//xTaskCreate(audio_left, "leftTask", 8192, NULL, 1, NULL);

	}
	if (isPlaying1 && playTimer1.done()) {
		// stop the audio and reset the flag
		isPlaying1 = false;
		audio.stopSong();
		motor1.stopMotor();
	}
	if (digitalRead(Input2) == LOW) {
		Serial.println("Input2 pressed");

	}
	vTaskDelay(1);
}
