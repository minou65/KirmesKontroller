// Blinker.h

#ifndef _BLINKER_h
#define _BLINKER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "LEDControl.h"
#include "neotimer.h"
#include "Vector.h"

class Ausgang : public LEDaccessories {
private:
	LED Output;

public:
	Ausgang(uint16_t BaseAddress_, uint8_t BaseChannel_);
	~Ausgang();

	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void SetMaxBrightness(uint16_t MaxBrightness) override;
	void on() override;
	void off() override;
};

//=======================================================
//Blinker mit 1 Lampen
//Mode 50
//Type n/a
//=======================================================
class Blinker : public LEDaccessories {
protected:
	LEDFader LED1;

	uint16_t timeOff;
	uint16_t timeOn;

	bool Status;

	Neotimer BlinkTimer;

public:
	Blinker(uint16_t BaseAddress_, uint8_t BaseChannel_, uint16_t timeOff_, uint16_t timeOn_);
	Blinker(uint16_t BaseAddress_, uint8_t BaseChannel_, uint16_t timeOff_, uint16_t timeOn_, uint8_t Mode_);
	Blinker(uint16_t BaseAddress_, uint8_t BaseChannel_, uint16_t timeOff_, uint16_t timeOn_, uint8_t fadeUpTime_, uint8_t fadeDownTime_, uint8_t Mode_);
	~Blinker();
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;

	void on() override;
	void off() override;
};

//=======================================================
//Wechselblinker
//Mode 51
//Type n/a
//=======================================================
class Wechselblinker : public Blinker {
private:
	LEDFader LED2;

public:
	Wechselblinker(uint16_t BaseAddress_, uint8_t BaseChannel_, uint16_t timeOff_, uint16_t timeOn_);
	Wechselblinker(uint16_t BaseAddress_, uint8_t BaseChannel_, uint16_t timeOff_, uint16_t timeOn_, uint8_t fadeUpTime_, uint8_t fadeDownTime_);
	~Wechselblinker();
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//Lauflicht
//Mode 
//Type 
//=======================================================
class Lauflicht : public LEDaccessories {
private:
	Vector<LEDFader*> LEDs;

	uint8_t NextStep;		// Welcher Ausgang als nächstes eingeschaltet werden soll
	uint8_t LastStep;		// Welcher Ausgang als letzer eingeschaltet wurde
	uint8_t Anzahl;		// Wieviele Ausgänge sollen verwendet werden

	bool Status;
	bool Direction;

	uint16_t timeOff;
	uint16_t timeOn;

	Neotimer OperationTimer;

public:
	Lauflicht(uint16_t BaseAddress_, uint8_t BaseChannel_, uint8_t Anzahl_, uint16_t timeOff_, uint16_t timeOn_, uint8_t Mode_);
	Lauflicht(uint16_t BaseAddress_, uint8_t BaseChannel_, uint8_t Anzahl_, uint16_t timeOff_, uint16_t timeOn_, uint8_t fadeUpTime_, uint8_t fadeDownTime_, uint8_t Mode_);
	~Lauflicht();
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//Hausbeleuchtung
//Mode 60
//Type n/a
// http://forum.arduino.cc/index.php?topic=159117.15
// Zeiten in Sekunden
//=======================================================
class Hausbeleuchtung : public LEDaccessories {
private:
	Vector<LED*> LEDs;

	uint16_t maxRandomTime;
	uint16_t minRandomTime;

	uint8_t Anzahl;		// Wieviele Ausgänge sollen verwendet werden

	Neotimer OperationTimer;

public:
	Hausbeleuchtung(uint16_t BaseAddress_, uint8_t BaseChannel_, uint8_t Anzahl_, uint32_t minRandomTime_, uint32_t maxRandomTime_);
	~Hausbeleuchtung();
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//Fernseher
//Mode 80
//Type n/a
// http://forum.arduino.cc/index.php?topic=159117.15
//=======================================================
class Fernseher : public Blinker {
public:
	Fernseher(uint16_t BaseAddress_, uint8_t BaseChannel_);
	~Fernseher();
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//Schweisslicht
//Mode 81
//Type n/a
// http://stummiforum.de/viewtopic.php?f=21&t=127899#p1446544
//=======================================================
class Schweissen : public Blinker {
private:
	LEDFader LED2;		// blau
	LEDFader LED3;		// rot

	// Definition der Flackerzeit
	uint8_t minRandomTime = 15;
	uint8_t maxRandomTime = 125;

	// Definition des Ausgabewertes für das Schweisslicht
	uint8_t flickermin = 25;
	uint8_t flickermax = 255;

	// Definition wie viele Flackern vor Pause
	uint8_t flickertime = 0;
	uint8_t flickertimemin = 100;
	uint8_t flickertimemax = 255;

	// Startwert Glühen und Delay
	uint8_t glow = 0;
	uint8_t glowdelay = 15;

	// Definition Länge der Pause
	uint16_t pauseMin;
	uint16_t pauseMax;

public:
	Schweissen(uint16_t BaseAddress_, uint8_t BaseChannel_, uint64_t minRandomPause_, uint64_t maxRandomPause_);
	~Schweissen();
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//NeonLampen
//Mode 61
//Type n/a
// http://forum.arduino.cc/index.php?topic=159117.15
// 
//=======================================================
class NeonLampen : public LEDaccessories {
private:
	bool Chance = 0;						// Prozentuale chance auf defekte Lampe: 
											// für 50:50 den Wert 1. für 33:66 den Wert 2, etc.
											// wenn 0 dann gibt es keine defekte Röhre
											// wenn 255, dann gibt es immer eine defekte Röhre

	Vector<Neon*> Lampen;
	const uint8_t Anzahl;

public:
	NeonLampen(uint16_t BaseAddress_, uint8_t BaseChannel_, const uint8_t Anzahl_, const uint8_t DefekteNeonChance_);
	~NeonLampen();
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//NatriumLampen
//Mode 62
//Type n/a
// 
//=======================================================
class NatriumLampen :public LEDaccessories {
private:
	uint8_t Chance = 0;					// Prozentuale chance auf defekte Lampe: 
											// für 50:50 den Wert 1. für 33:66 den Wert 2, etc.
											// wenn 0 dann gibt es keine defekte Röhre
											// wenn 255, dann gibt es immer eine defekte Röhre

	uint8_t fadeOnIntervall;
	uint8_t fadeOffIntervall;

	Vector<Natrium*> Lampen;
	const uint8_t Anzahl;

public:

	NatriumLampen(uint16_t BaseAddress_, uint8_t BaseChannel_, uint8_t Anzahl_, uint8_t Chance_, uint8_t fadeOnIntervall_, uint8_t fadeOffIntervall_);
	~NatriumLampen();
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void on() override;
	void off() override;
};

//=======================================================
//Feuer
//Mode 82
//Type n/a
// http://www.instructables.com/id/Realistic-Fire-Effect-with-Arduino-and-LEDs/?ALLSTEPS
//=======================================================
class Feuer : public Blinker {
private:
	// LEDFader m_LED1;		// gelb
	LEDFader LED2;		// rot
	LEDFader LED3;		// gelb

	// Definition der Flackerzeit
	uint8_t minRandomTime = 15;
	uint8_t maxRandomTime = 125;

public:
	Feuer(uint16_t BaseAddress_, uint8_t BaseChannel_);
	~Feuer();
	void SetMaxBrightness(uint16_t MaxBrightness);
	void process() override;
	void off() override;
};


class Blitzlicht : public Blinker {
private:
	Neotimer Blitztimer;
	bool Status2;

	uint32_t sleeptimeMin;
	uint32_t sleeptimeMax;
	uint16_t blitztimeMin;
	uint16_t blitztimeMax;

public:
	Blitzlicht(uint16_t BaseAddress_, uint8_t BaseChannel_);
	Blitzlicht(uint16_t BaseAddress_, uint8_t BaseChannel_, uint32_t sleeptimeMin_, uint32_t sleeptimeMax_, uint16_t blitztimeMin_, uint16_t blitztimeMax_);
	~Blitzlicht();

	void SetMaxBrightness(uint16_t MaxBrightness);

	void process() override;
	void on() override;
};


#endif

