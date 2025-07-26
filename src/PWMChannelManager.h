// PWMChannelManager.h

#ifndef _PWMCHANNELMANAGER_h
#define _PWMCHANNELMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class PWMChannelManager {
public:
    static const int NUM_CHANNELS = 16;

    PWMChannelManager() {
        for (int i = 0; i < NUM_CHANNELS; ++i) {
            used[i] = false;
        }
    }

    // Gibt einen freien Kanal zurück, oder -1 wenn keiner frei ist
    int allocate() {
        for (int i = 0; i < NUM_CHANNELS; ++i) {
            if (!used[i]) {
                used[i] = true;
                return i;
            }
        }
        return -1; // Kein Kanal frei
    }

    // Gibt einen Kanal wieder frei
    void free(int channel) {
        if (channel >= 0 && channel < NUM_CHANNELS) {
            used[channel] = false;
        }
    }

    // Prüfen ob ein Kanal belegt ist
    bool isUsed(int channel) const {
        if (channel >= 0 && channel < NUM_CHANNELS)
            return used[channel];
        return false;
    }

private:
    bool used[NUM_CHANNELS];
};

extern PWMChannelManager pwmManager;

#endif

