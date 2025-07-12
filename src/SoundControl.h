// SoundControl.h

#ifndef _SOUNDCONTROL_h
#define _SOUNDCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void setupSpund();
void loopSound();

#endif

