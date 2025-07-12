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

void audio_eof_mp3(const char* info) {
    Serial.print("eof: ");
    Serial.println(info);
    // Datei erneut abspielen (Endlosschleife)
}

void audio_info(const char* info) {
    Serial.print("info: ");
    Serial.println(info);
}
void audio_id3data(const char* info) {
    Serial.print("id3: ");
    Serial.println(info);
}


void audio_error(const char* info) {
    Serial.print("error: ");
    Serial.println(info);
}

void setupSpund() {
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



