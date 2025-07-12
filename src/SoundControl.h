// SoundControl.h

#ifndef _SOUNDCONTROL_h
#define _SOUNDCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void setupSound();
void loopSound();

void playSound(const char* filename);
void playSoundLoop(const char* filename);
void stopSound();

void setSoundVolume(int volume);
void setSoundBalance(int8_t balance);
void setSoundMono(bool mono);

bool isplayingSound();
bool isLoopModeSound();



#endif

