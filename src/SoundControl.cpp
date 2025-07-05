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

Sound::Sound(int bclk, int lrclk, int dout)
    : _bclk(bclk), _lrclk(lrclk), _dout(dout), _isPlaying(false) {}

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
}

bool Sound::isPlaying() const {
    return _isPlaying;
}

void Sound::setVolume(uint8_t Volume) {
    if (Volume > 21)
        Volume = 21;
    _audio..setVolume(Volume);
}

void Sound::setBalance(int8_t Balance) {
    if (Balance > 16)
        Balance = 16;
    if (Balance < -16)
        Balance = -16;
     _audio.setBalance(-16); // -16...16 (left to right)
}

