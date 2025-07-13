// 
// 
// 

#include "SoundControl.h"
#include "pinmapping.h"
#include <audio.h>


#include <FS.h>
#include <SD.h>
#include <SPI.h>

SPIClass* spi_onboardSD = new SPIClass(FSPI);
Audio     audio;
File      root;

bool isPlaying = false;
bool isLoopMode = false;

void audio_eof_mp3(const char* info) {
    Serial.print("eof: "); Serial.println(info);
    if (isLoopMode) {
		Serial.println("Looping sound...");
        playSound(info);
    } else {
        Serial.println("Sound playback finished.");
	}
}

void audio_info(const char* info) {
    Serial.print("info: "); Serial.println(info);
}
void audio_id3data(const char* info) {
    Serial.print("id3: "); Serial.println(info);
}


void audio_error(const char* info) {
    Serial.print("error: "); Serial.println(info);
}

void setupSound() {
	Serial.println("Setting up sound system...");
    spi_onboardSD->begin();

    if (!SD.begin(SS, *spi_onboardSD)) {
        Serial.println("error mounting microSD");
        return;
    }
    Serial.println("microSD mounted successfully");

	audio.setPinout(I2S_BCLK, I2S_LRCLK, I2S_DOUT);
    audio.setVolume(21); // Set volume to maximum (0-21)
    audio.setBalance(0); // Center balance
    audio.forceMono(false); // Stereo output
	Serial.println("Sound system setup complete.");
}

void loopSound() {
    audio.loop();
    vTaskDelay(1);
}

void playSound(const char* filename) {
    if (audio.connecttoFS(SD, filename)) {
        isPlaying = true;
    }
    else {
        Serial.println("Failed to play file");
        isPlaying = false;
    }
}

void playSoundLoop(const char* filename) {
    isLoopMode = true;
    playSound(filename);
}


void stopSound() {
    audio.stopSong();
    isPlaying = false;
    isLoopMode = false;
}

void setSoundVolume(int volume) {
    if (volume > 21)
        volume = 21;
    audio.setVolume(volume);
}

void setSoundBalance(int8_t balance) {
    if (balance < -16)
        balance = -16;
    if (balance > 16)
        balance = 16;
    audio.setBalance(balance);
}

void setSoundMono(bool mono) {
    audio.forceMono(mono);
}

bool isplayingSound() {
    return isPlaying;
}

bool isLoopModeSound() {
    return isLoopMode;
}


