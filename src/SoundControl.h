// SoundControl.h

#ifndef _SOUNDCONTROL_h
#define _SOUNDCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Audio.h>

class Sound {
public:
    Sound(int bclk, int lrclk, int dout);

    Sound();

    bool begin();

    void play(const char* filename);
    void stop();

    void process();

    bool isPlaying() const;

    void setVolume(uint8_t Volume);
    void setBalance(int8_t Balance);

private:
    int _bclk, _lrclk, _dout;
    bool _isPlaying;
    Audio _audio;
};

#endif

