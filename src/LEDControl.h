// LEDControl.h

#ifndef _LEDCONTROL_h
#define _LEDCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include "WProgram.h"
#endif

#include "neotimer.h"
#include "accessories.h"

// Values for On and Off
// Werden die LED's direkt am ESP angeschlossen sind die 
// Werte zu invertieren. O = on, 255 = off
#define PWM_Set_On 255
#define PWM_Set_Off 0

class LEDaccessories : public Accessory {
public:
	LEDaccessories() = default;
	LEDaccessories(uint16_t BaseAddress, byte BaseChannel);
	LEDaccessories(uint16_t BaseAddress, byte BaseChannel, byte Mode);
	~LEDaccessories();
	AccessoryType getType() const override;
	virtual void SetMaxBrightness(uint16_t MaxBrightness);
};

// ===========================================
// LED
// ===========================================
class LED {
protected:
	uint8_t _GPIO;
	bool _IsActive;

	uint16_t _MaxBrightness = PWM_Set_On;		// On Value

public:
	LED(const uint8_t Channel);
	LED(const uint8_t Channel, uint8_t Brightness);
	virtual ~LED();

	bool attach(uint8_t pin);
	void detach();
	void write(uint16_t value);

	// Muss regelmässig aufgerufen werden
	virtual void process();

	virtual void SetMaxBrightness(uint16_t MaxBrightness);

	void on();
	void off();
	bool isOn();

private:
	bool _isAttached = false;

	uint8_t _Channel;	
	uint16_t _PWMFrequency;
	uint16_t _PWMResolution;

};

// ===========================================
// LEDFader
// ===========================================
class LEDFader : public LED {
protected:
	Neotimer _fadeUpTimer;
	Neotimer _fadeDownTimer;

	int16_t _CurrentBrightness;			// Aktuelle Wert für LED beim faden
	int16_t _TargetBrightness;			// Wert, welcher eingestellt werden soll

	// Time in ms
	uint16_t _fadeUpIntervall;
	uint16_t _fadeDownIntervall;

	// Anzahl Steps um die die Helligkeit der LED pro Intervall erhöht wird 1-255
	// Ist der Wert 0 wird direkt auf Helligkeit direkt auf Target gesetzt
	uint8_t _fadeUpRate;
	uint8_t _fadeDownRate;

	// Um besonders hohe Zeiten zu erreichen wird Intervall mit diesem Wert multipliziert. Default ist 1
	uint8_t _FadeMultiplikator;

public:

	LEDFader(const uint8_t Channel);
	LEDFader(const uint8_t Channel, uint8_t Brightness, uint16_t fadeUpTime = 1000, uint16_t fadeDownTime = 1000);
	~LEDFader() override;

	// Muss regelmässig aufgerufen werden
	void process();

	void on();
	void on(uint8_t Brightness);
	void off();

	// Brightness in Prozent 0 - 100; , wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
	void SetPercent(uint8_t Percent, bool Hardset = false);
	// Brightness von 0 - 255, wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
	void SetBrightness(uint16_t Brightness, bool Hardset = false);
	// Setzen der maximalen Helligkeit
	void SetMaxBrightness(uint16_t MaxBrightness);

	// Time in ms
	void SetFadeTime(uint16_t fadeUpTime, uint16_t fadeDownTime);
	// Time in ms, zudem kann das Intevall angepasst werden, kleinst möglicher Wert ist 10ms
	void SetFadeTime(uint16_t fadeUpTime, uint16_t fadeDownTime, uint16_t fadeUpInterval, uint16_t fadeDownInterval);
	// Mit diser funktion kann das Intervall zusätzlich erhöht werden. Sollte nicht mehr verwendet werden
	void SetFadeMultiplikator(uint8_t Multiplikator);
	// Liefert die aktuelle helligkeit
	uint8_t GetCurrentBrightness();
	// Prüft ob die LED ganz Dunkel ist
	bool isDark();
private:
	friend class LED;
};

// ===========================================
// Simuliert eine Natriumdampflampe
// ===========================================
class Natrium : protected LEDFader {
protected:
	enum status {
		GlowingOn = 1,
		OperatingPressure = 2,
		On = 3,
		GlowingOff = 4,
		Off = 5
	};

	status _CurrentStatus = status::Off;

	const uint16_t _OperationTime = 500;

	uint16_t _MalFunctionInterval;
	const uint16_t _MalFunctionIntervalMin = 5000;
	const uint16_t _MalFunctionIntervalMax = 60000;
	bool _IsMalFunction;

	const uint8_t _randomMax = 15;		// Definition in Prozent wie stark die Lampe beim
	const uint8_t _randomMin = 5;		// Ein- und Ausschalten noch leuchten soll
	// Danach dimmt die Lampe zu 100% oder zu 0%

	Neotimer _Malfunctiontimer;
	Neotimer _Operationtimer;

public:
	Natrium(const uint8_t Channel, const bool MalFunction, uint8_t fadeOnIntervall, uint8_t fadeOffIntervall);
	~Natrium() override;
	void SetMaxBrightness(uint16_t MaxBrightness) override;
	void process();
	void on();
	void off();
	void off(bool force);
};

// ===========================================
// Simuliert eine Neon Röhre
// ===========================================
class Neon : protected LED {
protected:
	bool _CurrentStatus = false;

	const bool _IsMalFunction;

	uint8_t _FlashCounter;			// Wie oft soll beim einschalten geblinkt werden

	Neotimer _Operationtimer;

public:
	Neon(const uint8_t Channel, const bool MalFunction);
	~Neon() override;
	void SetMaxBrightness(uint16_t MaxBrightness) override;
	void process();
	void on();
	void off();
};

#endif

