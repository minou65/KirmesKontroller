// 
// 
// 

#include "SoundControl.h"
#include "pinmapping.h"
#include <audio.h>


#include <FS.h>
#include <SPI.h>
#include <SD_MMC.h>
#include <Audio.h>

#include <SD_MMC.h>


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

Sound::Sound(int bclk, int lrclk, int dout) : 
    _bclk(bclk), 
    _lrclk(lrclk), 
    _dout(dout), 
    _isPlaying(false),
    _loopMode(false) {
}

Sound::Sound() {
    Sound(I2S_BCLK, I2S_LRCLK, I2S_DOUT);
}

bool Sound::begin() {
    SD_MMC.setPins(SPI_SCK, SPI_MOSI, SPI_MISO);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("Card Mount Failed");
        return false;
    }
    _audio.setPinout(_bclk, _lrclk, _dout);
    return true;
}

void Sound::play(const char* filename) {
    strncpy(_lastPlayedFile, filename, sizeof(_lastPlayedFile));
    if (_audio.connecttoFS(SD_MMC, filename)) {
        _isPlaying = true;
    }
    else {
        Serial.println("Failed to play file");
        _isPlaying = false;
    }
}

void Sound::stop() {
    _audio.stopSong();
    _isPlaying = false;
}

void Sound::process() {
    _audio.loop();
    if(_loopMode && !_audio.isRunning()) {
        // If loop mode is enabled and the audio is not running, restart playback
        play(_lastPlayedFile);
	}
}

bool Sound::isPlaying() const {
    return _isPlaying;
}

void Sound::setVolume(uint8_t Volume) {
    if (Volume > 21)
        Volume = 21;
    _audio.setVolume(Volume);
}

void Sound::setBalance(int8_t Balance) {
    if (Balance > 16)
        Balance = 16;
    if (Balance < -16)
        Balance = -16;
     _audio.setBalance(-16); // -16...16 (left to right)
}

void Sound::setLoopMode(bool loop) {
    _loopMode = loop;
}

void Sound::setMono(bool mono) {
    _audio.forceMono(mono);

}


