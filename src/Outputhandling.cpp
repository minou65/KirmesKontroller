// 
// 
// 

#include "Outputhandling.h"
#include "LEDControl.h"

using namespace std;

//=======================================================
//einfacher Ausgang
//Mode 40
//Type n/a
//=======================================================
Ausgang::Ausgang(uint16_t BaseAddress_, uint8_t BasePin) :
	LEDaccessories(BaseAddress_, BasePin, 40),
	Output(BasePin) {

	off();
};

Ausgang::~Ausgang() {
}

void Ausgang::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	if (Address_ == BaseAddress) {
		if (IsActive != static_cast<bool>(cmd_)) {
			Serial.println("Ausgang::notifyAddress");
			Serial.print("    Address:     "); Serial.println(Address_, DEC);
			Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);

			if (static_cast<bool>(cmd_)) {
				on();
			}
			else {
				off();
			}
		}
	}
}

void Ausgang::SetMaxBrightness(uint16_t MaxBrightness) {
	Output.SetMaxBrightness(MaxBrightness);
}

void Ausgang::on() {
	Serial.println("Ausgang::on");
	IsActive = true;
	Output.on();
	Accessory::on();
};

void Ausgang::off() {
	Serial.println("Ausgang::off");
	IsActive = false;
	Output.off();
	Accessory::off();
};

void Ausgang::process() {
	Accessory::process();
	Output.process();
}


//=======================================================
//Blinker mit 1 Lampen
//Mode 50
//Type n/a
//=======================================================
Blinker::Blinker(uint16_t BaseAddress_, uint8_t BasePin, uint16_t timeOff_, uint16_t timeOn_) :
	LEDaccessories(BaseAddress_, BasePin, 50),
	Status(false),
	timeOff(timeOff_),
	timeOn(timeOn_),
	LED1(BasePin, PWM_Set_On, 0, 0) {

	off();
};

Blinker::Blinker(uint16_t BaseAddress_, uint8_t BasePin, uint16_t timeOff_, uint16_t timeOn_, uint8_t Mode_) :
	LEDaccessories(BaseAddress_, BasePin, Mode_),
	Status(false),
	timeOff(timeOff_),
	timeOn(timeOn_),
	LED1(BasePin, PWM_Set_On, 10, 10) {

	off();

};

Blinker::Blinker(uint16_t BaseAddress_, uint8_t BasePin, uint16_t timeOff_, uint16_t timeOn_, uint8_t fadeUpTime_, uint8_t fadeDownTime_, uint8_t Mode_) :
	LEDaccessories(BaseAddress_, BasePin, Mode_),
	Status(false),
	timeOff(timeOff_),
	timeOn(timeOn_),
	LED1(BasePin, PWM_Set_On, fadeUpTime_, fadeDownTime_) {

	off();
}
Blinker::~Blinker() {
	LED1.~LEDFader();
	BlinkTimer.~Neotimer();
}

void Blinker::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	if (Address_ == BaseAddress) {
		if (IsActive != static_cast<bool>(cmd_)) {
			Serial.println("Blinker::notifyAddress");
			Serial.print("    Address:     "); Serial.println(Address_, DEC);
			Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);

			if (static_cast<bool>(cmd_)) {
				on();
			}
			else {
				off();
			}
		}
	}
}

void Blinker::SetMaxBrightness(uint16_t MaxBrightness) {
	LED1.SetMaxBrightness(MaxBrightness);
}

void Blinker::process() {
	LED1.process();
	LEDaccessories::process();

	// do nothing if not active
	if (IsActive) {

		if (BlinkTimer.repeat()) {
			if (Status) {
				LED1.on();
				BlinkTimer.start(timeOn);
			}
			else {
				LED1.off();
				BlinkTimer.start(timeOff);
			}
			Status = !Status;
		}

	}
}

void Blinker::on() {
	Serial.println("Blinker::on");
	Serial.print("   timeOn:  "); Serial.println(timeOn);
	Serial.print("   timeOff: "); Serial.println(timeOff);
	IsActive = true;
	Status = true;
	BlinkTimer.start(0);
	LEDaccessories::on();
}

void Blinker::off() {
	Serial.println("Blinker::off");
	IsActive = false;
	BlinkTimer.stop();
	LED1.off();
	LEDaccessories::off();
}

//=======================================================
//Wechselblinker
//Mode 51
//Type n/a
//=======================================================
Wechselblinker::Wechselblinker(uint16_t BaseAddress_, uint8_t BasePin, uint16_t timeOff_, uint16_t timeOn_) :
	Blinker(BaseAddress_, BasePin, timeOff_, timeOn_, 51),
	LED2(BasePin + 1, PWM_Set_On, 0, 0) {
};

Wechselblinker::Wechselblinker(uint16_t BaseAddress_, uint8_t BasePin, uint16_t timeOff_, uint16_t timeOn_, uint8_t fadeUpTime_, uint8_t fadeDownTime_) :
	Blinker(BaseAddress_, BasePin, timeOff_, timeOn_, 51),
	LED2(BasePin + 1, PWM_Set_On, fadeUpTime_, fadeDownTime_) {

	LED1.SetFadeMultiplikator(1);
	LED2.SetFadeMultiplikator(1);
	LED1.SetFadeTime(fadeUpTime_, fadeDownTime_, 10, 10);
	LED2.SetFadeTime(fadeUpTime_, fadeDownTime_, 10, 10);
}

Wechselblinker::~Wechselblinker() {
	LED2.~LEDFader();
}

void Wechselblinker::SetMaxBrightness(uint16_t MaxBrightness) {
	Blinker::SetMaxBrightness(MaxBrightness);
	LED2.SetMaxBrightness(MaxBrightness);
}

void Wechselblinker::process() {
	LED1.process();
	LED2.process();

	// do nothing if not active
	if (IsActive) {
		if (BlinkTimer.repeat()) {
			if (Status) {
				LED1.on();
				LED2.off();
				BlinkTimer.start(timeOn);
			}
			else {
				LED1.off();
				LED2.on();
				BlinkTimer.start(timeOff);
			}

			Status = !Status;
		}
	}
	Accessory::process();
}

void Wechselblinker::on() {
	Serial.println("Wechselblinker::on");
	Serial.print("   timeOn:  "); Serial.println(timeOn);
	Serial.print("   timeOff: "); Serial.println(timeOff);
	Blinker::on();
	Accessory::on();
}

void Wechselblinker::off() {
	Serial.println("Wechselblinker::off");
	IsActive = false;
	LED1.off();
	LED2.off();
	Accessory::off();
}

//=======================================================
//Lauflicht
//Mode 
//Type 
//=======================================================
Lauflicht::Lauflicht(uint16_t BaseAddress_, uint8_t BasePin, uint8_t Anzahl_, uint16_t timeOff_, uint16_t timeOn_, uint8_t Mode_) :
	LEDaccessories(BaseAddress_, BasePin, Mode_),
	Anzahl(Anzahl_),
	Status(false),
	Direction(true),
	timeOn(timeOn_),
	timeOff(timeOff_) {

	// Anzahl LED's erstellen
	for (int _i = 0; _i < Anzahl_; _i++) {
		LEDs.PushBack(new LEDFader(BasePin + _i, PWM_Set_On, 0, 0));
	}

	off();
};

Lauflicht::Lauflicht(uint16_t BaseAddress_, uint8_t BasePin, uint8_t Anzahl_, uint16_t timeOff_, uint16_t timeOn_, uint8_t fadeUpTime_, uint8_t fadeDownTime_, uint8_t Mode_) :
	LEDaccessories(BaseAddress_, BasePin, Mode_),
	Anzahl(Anzahl_),
	Status(false),
	Direction(true),
	timeOn(timeOn_),
	timeOff(timeOff_) {

	// Anzahl LED's erstellen
	for (int _i = 0; _i < Anzahl_; _i++) {
		LEDs.PushBack(new LEDFader(BasePin + _i, PWM_Set_On, fadeUpTime_, fadeDownTime_));
	};

	off();
};

Lauflicht::~Lauflicht() {
	off();

	for (int i = 0; i < LEDs.Size(); i++) {
		LEDs[i]->~LEDFader();
		delete LEDs[i];
	};

	// Grösse von Decoder auf 0 setzen
	LEDs.Clear();

	// Speicher von decoder freigeben
	Vector<LEDFader*>().Swap(LEDs);

	OperationTimer.~Neotimer();
	LEDs.~Vector();
};

void Lauflicht::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	if (Address_ == BaseAddress) {
		if (IsActive != static_cast<bool>(cmd_)) {
			Serial.println("Lauflicht::notifyAddress");
			Serial.print("    Address:     "); Serial.println(Address_, DEC);
			Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);

			if (static_cast<bool>(cmd_)) {
				on();
			}
			else {
				off();
			}
		}
	}
}

void Lauflicht::SetMaxBrightness(uint16_t MaxBrightness) {
	for (int _i = 0; _i < LEDs.Size(); _i++)
		LEDs[_i]->SetMaxBrightness(MaxBrightness);
}

void Lauflicht::process() {
	for (int _i = 0; _i < LEDs.Size(); _i++)
		LEDs[_i]->process();

	// do nothing if not active
	if (IsActive) {
		// if time to do something, do it
		if (OperationTimer.repeat()) {

			switch (Mode) {
			case 52:
				// Kette durchgehen und notwendige LED einschalten, die anderen aussschalten
				if (LastStep != NextStep) {
					LEDs[LastStep]->off();
					LastStep = NextStep;
				};

				LEDs[NextStep]->on(PWM_Set_On);
				NextStep++;

				if (NextStep >= Anzahl)
					NextStep = 0;

				break;
			case 53:
				// Alle Lampen werden nacheinander eingschaltet, am Schluss werden alle ausgeschaltet

				// Ende der Kette ist noch nicht erreicht
				if (NextStep != 254) {

					if (NextStep < Anzahl)
						LEDs[NextStep]->on();
					NextStep++;

					if (NextStep > Anzahl)
						NextStep = 254;
				}
				// Ende der Kette ist erreicht
				else {
					off();
					IsActive = true;

					// Prüfen ob alle Lampen dunkel sind
					bool _b = true;

					for (int _i = 0; _i < LEDs.Size(); _i++) {
						if (LEDs[_i]->isDark()) {
							_b = false;
						}
					}

					// Wenn alle dunkel sind  dann nächster Step
					if (_b)
						NextStep = 0;
				};
				break;

			case 54:
				// Lampe für Lampe wird eingeschaltet. Sind alle eingeschaltet, wird mit der ersten Lampe begonnen zu löschen.
				if (NextStep < Anzahl)
				{
					LEDs[NextStep]->on();
					NextStep++;

				}
				else if (LastStep < Anzahl) {
					LEDs[LastStep]->off();
					LastStep++;
				}
				else {
					NextStep = 0;
					LastStep = 0;
				};

				break;

			case 55:
				// Kette durchgehen und notwendige LED einschalten, die anderen aussschalten
				if (LastStep != NextStep) {
					LEDs[LastStep]->off();
					LastStep = NextStep;
				};

				LEDs[NextStep]->on(PWM_Set_On);

				if (Direction) {
					NextStep++;
				}
				else {
					NextStep--;
				}

				if (NextStep >= Anzahl && Direction) {
					NextStep = Anzahl - 1;
					Direction = false;
				}

				if (NextStep <= 0 && !Direction) {
					NextStep = 0;
					Direction = true;
				}

				break;

			};

			// Set on or off Intervall
			if (Status) {
				OperationTimer.start(timeOn);
			}
			else {
				OperationTimer.start(timeOff);;
			};

			Status = !Status;

		}; // end of if
	}
	Accessory::process();
};

void Lauflicht::on() {
	Serial.println("Lauflicht::on");
	Serial.print("    Mode: "); Serial.println(Mode);
	IsActive = true;
	OperationTimer.start(timeOn);
	Status = false;
	NextStep = 0;
	LastStep = 0;
	Accessory::on();
}

void Lauflicht::off() {
	Serial.println("Lauflicht::off");
	IsActive = false;

	for (int i = 0; i < LEDs.Size(); i++) {
		LEDs[i]->off();
	}
	Accessory::off();
}

//=======================================================
//Hausbeleuchtung
//Mode 60
//Type n/a
// http://forum.arduino.cc/index.php?topic=159117.15
//=======================================================
Hausbeleuchtung::Hausbeleuchtung(uint16_t BaseAddress_, uint8_t BasePin, uint8_t Anzahl_, uint32_t minRandomTime_, uint32_t maxRandomTime_) :
	LEDaccessories(BaseAddress_, BasePin),
	Anzahl(Anzahl_),
	maxRandomTime(maxRandomTime_),
	minRandomTime(minRandomTime_) {

	// Anzahl LED's erstellen
	for (int i_ = 0; i_ < Anzahl_; i_++) {
		LEDs.PushBack(new LED(BasePin + i_));
	};

	// Default für m_minRandomTime setzen wenn 0
	if (!minRandomTime)
		minRandomTime = 5;

	// Default für m_maxRandomTime setzen wenn 0
	if (!maxRandomTime)
		maxRandomTime = 20;

	off();
};

Hausbeleuchtung::~Hausbeleuchtung() {

	off();

	for (int i = 0; i < LEDs.Size(); i++) {
		LEDs[i]->~LED();
		delete LEDs[i];
	};

	// Grösse von Decoder auf 0 setzen
	LEDs.Clear();

	// Speicher von decoder freigeben
	Vector<LED*>().Swap(LEDs);

	OperationTimer.~Neotimer();
	LEDs.~Vector();
};

void Hausbeleuchtung::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	if (Address_ == BaseAddress) {
		if (IsActive != static_cast<bool>(cmd_)) {
			Serial.println("Hausbeleuchtung::notifyAddress");
			Serial.print("    Address:     "); Serial.println(Address_, DEC);
			Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);

			if (static_cast<bool>(cmd_)) {
				on();
			}
			else {
				off();
			}
		}
	}
}

void Hausbeleuchtung::SetMaxBrightness(uint16_t MaxBrightness) {
	for (int _i = 0; _i < LEDs.Size(); _i++)
		LEDs[_i]->SetMaxBrightness(MaxBrightness);
}

void Hausbeleuchtung::process() {
	for (int i = 0; i < LEDs.Size(); i++) {
		LEDs[i]->process();
	}

	// do nothing if not active
	if (IsActive) {
		// if time to do something, do it
		if (OperationTimer.repeat()) {
			uint8_t t1_ = 0;
			uint8_t	t2_ = 0;

			// Bestimmen welcher Ausgang geschaltet werden soll
			randomSeed(esp_random());
			t2_ = random(Anzahl);

			// Bestimmen wann das nächstemal etwas gemacht werden soll
			OperationTimer.start(random(minRandomTime, maxRandomTime));

			for (int i = 0; i < LEDs.Size(); i++) {

				bool on_ = LEDs[i]->isOn();

				if (!on_ && IsActive && (t1_ == t2_)) {
					LEDs[i]->on();
					break;
				};

				if ((on_) && (t1_ == t2_)) {
					LEDs[i]->off();
					break;
				};

				t1_++;
			};
		};
	}
	Accessory::process();

};

void Hausbeleuchtung::on() {
	Serial.println("Hausbeleuchtung::on");
	IsActive = true;
	randomSeed(esp_random());
	OperationTimer.start(random(minRandomTime, maxRandomTime));
	Accessory::on();
}

void Hausbeleuchtung::off() {
	Serial.println("Hausbeleuchtung::off");
	IsActive = false;
	for (int i = 0; i < LEDs.Size(); i++) {
		LEDs[i]->off();
	}
	OperationTimer.stop();
	Accessory::off();
}

//=======================================================
//Fernseher
//Mode 80
//Type n/a
// http://forum.arduino.cc/index.php?topic=159117.15
//=======================================================
Fernseher::Fernseher(uint16_t BaseAddress_, uint8_t BasePin) :
	Blinker(BaseAddress_, BasePin, 0, 0, 0, 0, 80) {

	randomSeed(esp_random());
}
Fernseher::~Fernseher() {

}

void Fernseher::process() {
	LED1.process();

	// do nothing if not active
	if (IsActive) {

		// if time to do something, do it
		if (BlinkTimer.repeat()) {
			LED1.SetBrightness(random(250), true);
			BlinkTimer.start(random(200));
		}
	}
	Accessory::process();
};

void Fernseher::on() {
	Serial.println("Fernseher::on");
	randomSeed(esp_random());
	IsActive = true;
	Status = true;
	LED1.on(random(250));
	BlinkTimer.start(random(200));
	Accessory::on();
};

void Fernseher::off() {
	Serial.println("Fernseher::off");
	IsActive = false;
	LED1.off();
	Accessory::off();
}

//=======================================================
//Schweisslicht
//Mode 81
//Type n/a
// http://stummiforum.de/viewtopic.php?f=21&t=127899#p1446544
//=======================================================
Schweissen::Schweissen(uint16_t BaseAddress_, uint8_t BasePin, uint64_t minRandomPause_, uint64_t maxRandomPause_) :
	Blinker(BaseAddress_, BasePin, 0, 0, 0, 0, 81),
	LED2(BasePin + 1, PWM_Set_On, 0, 0),
	LED3(BasePin + 2, PWM_Set_On, 0, 0) {

	pauseMin = minRandomPause_ * 10;
	pauseMax = maxRandomPause_ * 10;

	LED1.off();
	LED2.off();
	LED3.off();

};

Schweissen::~Schweissen() {
	LED2.~LEDFader();
	LED3.~LEDFader();
}

void Schweissen::SetMaxBrightness(uint16_t MaxBrightness) {
	LED1.SetMaxBrightness(MaxBrightness);
	LED2.SetMaxBrightness(MaxBrightness);
	LED3.SetMaxBrightness(MaxBrightness);

}

void Schweissen::process() {
	LED1.process();
	LED2.process();
	LED3.process();

	// do nothing if not active and not is running
	if (IsActive && Status) {

		// if time to do something, do it
		if (BlinkTimer.repeat()) {
			randomSeed(esp_random());

			// Solange Flickertime grösser als 0 ist, schweissen
			if (flickertime > 0) {

				// Flickertime reduzieren
				flickertime--;

				// running State auf true
				Status = true;

				// Flackerwert bestimmen
				uint8_t _flicker = random(flickermin, flickermax);

				LED1.SetBrightness(_flicker, true);
				LED2.SetBrightness(_flicker, true);

				// Je länger geschweisst wird, desto heisser wird das Material
				if (PWM_Set_Off == 0) {
					glow++;
				}
				else {
					glow--;
				}

				LED3.SetBrightness(glow, true);

				// delay bestimmen
				BlinkTimer.start(random(minRandomTime, maxRandomTime));

				if (flickertime == 0) {
					// Es wird nicht mehr geschweisst
					LED1.SetBrightness(PWM_Set_Off, true);
					LED2.SetBrightness(PWM_Set_Off, true);
				};

			} // if (m_flickertime > 0)
			else if (glow != PWM_Set_Off) {

				if (PWM_Set_Off == 0) {
					glow--;
				}
				else {
					glow++;
				}

				// Langsam ausglühen
				LED3.SetBrightness(glow, true);

				// delay bestimmen
				BlinkTimer.start(random(glowdelay));

				// Pause bis zum nächsten Schweissen bestimmen
				if (glow == PWM_Set_Off) {
					BlinkTimer.start(random(pauseMin, pauseMax));
				};

			} // else if (m_glow > 0)
			else if (glow == PWM_Set_Off) {
				// Schweissvorgang neu starten
				BlinkTimer.start(random(flickertimemin, flickertimemax));

				// Anzahl flicker
				flickertime = random(flickertimemin, flickertimemax);

				// Running State auf false
				Status = false;
			};
		};
	}
	Accessory::process();
};

void Schweissen::on() {
	Serial.println("Schweissen::on");
	IsActive = true;
	Status = true;
	BlinkTimer.start(timeOn);

	// running State auf true
	Status = true;
	glow = PWM_Set_Off;

	randomSeed(esp_random());

	LED1.on(random(flickermin, flickermax));
	LED2.on(random(flickermin, flickermax));
	LED3.on(glow);

	// delay bestimmen
	BlinkTimer.start(random(minRandomTime, maxRandomTime));

	// Anzahl flicker
	flickertime = random(flickertimemin, flickertimemax);
	Accessory::on();
};

void Schweissen::off() {
	Serial.println("Schweissen::off");
	IsActive = false;
	Accessory::off();
}

//=======================================================
//NeonLampen
//Mode 61
//Type n/a
// http://forum.arduino.cc/index.php?topic=159117.15
// 
//=======================================================
NeonLampen::NeonLampen(uint16_t BaseAddress_, uint8_t BasePin, uint8_t Anzahl_, uint8_t Chance_) :
	LEDaccessories(BaseAddress_, BasePin),
	Anzahl(Anzahl_),
	Chance(Chance_) {

	uint8_t DefekteLampe_ = 0;
	bool Defekt_ = false;

	randomSeed(esp_random());
	// Chance_
	// Prozentuale chance auf defekte Lampe: 
	// für 50:50 den Wert 1. für 33:66 den Wert 2, etc.
	// wenn Null dann gibt es keine defekte Lampe
	// wenn 255, dann gibt es immer eine defekte Lampe
	if (((Chance >= 1) && (random(0, 1 + Chance) == 1)) || (Chance == 255)) {
		// Wir haben eine defekte Lampe
		Defekt_ = true;
		// Welche Lampe ist defekt?
		DefekteLampe_ = random(0, Anzahl);
	}

	// Anzahl Lampen erstellen
	for (int i_ = 0; i_ < Anzahl; i_++) {
		if (Defekt_ && (i_ == DefekteLampe_))
			Lampen.PushBack(new Neon(BaseChannel + i_, true));
		else
			Lampen.PushBack(new Neon(BaseChannel + i_, false));
	}

	off();
}

NeonLampen::~NeonLampen() {
	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->~Neon();
		delete Lampen[i];
	}

	// Grösse von Decoder auf 0 setzen
	Lampen.Clear();

	// Speicher von decoder freigeben
	Vector<Neon*>().Swap(Lampen);
	Lampen.~Vector();

}

void NeonLampen::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	if (Address_ == BaseAddress) {
		if (IsActive != static_cast<bool>(cmd_)) {
			Serial.println("Neonlampen::notifyAddress");
			Serial.print("    Address:     "); Serial.println(Address_, DEC);
			Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);

			if (static_cast<bool>(cmd_)) {
				on();
			}
			else {
				off();
			}
		}
	}
}

void NeonLampen::SetMaxBrightness(uint16_t MaxBrightness) {
	for (int _i = 0; _i < Lampen.Size(); _i++)
		Lampen[_i]->SetMaxBrightness(MaxBrightness);
}

void NeonLampen::process() {

	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->process();
	}
	Accessory::process();
}

void NeonLampen::on() {
	Serial.println("NeonLampe::on");
	IsActive = true;
	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->on();
	}
	Accessory::on();
}

void NeonLampen::off() {
	Serial.println("NeonLampe::off");
	IsActive = false;
	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->off();
	}
	Accessory::off();
}

//=======================================================
//NatriumLampen
//Mode 62
//Type n/a
// 
//=======================================================
NatriumLampen::NatriumLampen(uint16_t BaseAddress_, uint8_t BasePin, uint8_t Anzahl_, uint8_t Chance_, uint8_t fadeOnIntervall_ = 10, uint8_t fadeOffIntervall_ = 10) :
	LEDaccessories(BaseAddress_, BasePin),
	Anzahl(Anzahl_),
	Chance(Chance_),
	fadeOnIntervall(fadeOnIntervall_),
	fadeOffIntervall(fadeOffIntervall_) {

	uint8_t _DefekteLampe = 0;
	bool _Defekt = false;

	if (!fadeOnIntervall)
		fadeOnIntervall = 1;

	if (!fadeOffIntervall)
		fadeOffIntervall = 1;

	randomSeed(esp_random());
	// Chance_
	// Prozentuale chance auf defekte Lampe: 
	// für 50:50 den Wert 1. für 33:66 den Wert 2, etc.
	// wenn Null dann gibt es keine defekte Lampe
	// wenn 255, dann gibt es immer eine defekte Lampe
	if (((Chance >= 1) && (random(0, 1 + Chance) == 1)) || (Chance == 255)) {
		// Wir haben eine defekte Lampe
		_Defekt = true;
		// Welche Lampe ist defekt?
		_DefekteLampe = random(0, Anzahl);
	}

	// Anzahl Lampen erstellen
	for (int _i = 0; _i < Anzahl; _i++) {

		if (_Defekt && (_i == _DefekteLampe))
			Lampen.PushBack(new Natrium(BaseChannel + _i, true, fadeOnIntervall, fadeOffIntervall));
		else
			Lampen.PushBack(new Natrium(BaseChannel + _i, false, fadeOnIntervall, fadeOffIntervall));

	}

	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->off(true);
	}

}

NatriumLampen::~NatriumLampen() {
	off();

	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->~Natrium();
		delete Lampen[i];
	};

	// Grösse von Decoder auf 0 setzen
	Lampen.Clear();

	// Speicher von decoder freigeben
	Vector<Natrium*>().Swap(Lampen);
	Lampen.~Vector();
}

void NatriumLampen::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	if (Address_ == BaseAddress) {
		if (IsActive != static_cast<bool>(cmd_)) {
			Serial.println("Neonlampen::notifyAddress");
			Serial.print("    Address:     "); Serial.println(Address_, DEC);
			Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);

			if (static_cast<bool>(cmd_)) {
				on();
			}
			else {
				off();
			}
		}
	}
}

void NatriumLampen::SetMaxBrightness(uint16_t MaxBrightness) {
	for (int _i = 0; _i < Lampen.Size(); _i++)
		Lampen[_i]->SetMaxBrightness(MaxBrightness);

}

void NatriumLampen::process() {
	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->process();
	}
	Accessory::process();
}

void NatriumLampen::on() {
	Serial.println("NatriumLampen::on");
	IsActive = true;

	// Lampen leuchten mit 15 - 25 % beim einschalten, danach auf 100%
	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->on();
	}
	Accessory::on();
}

void NatriumLampen::off() {
	Serial.println("NatriumLampen::off");
	IsActive = false;

	// Lampen auschalten, sie leuchten noch mit 15 -25% und leuchten dann langsam aus.
	for (int i = 0; i < Lampen.Size(); i++) {
		Lampen[i]->off();
	}
	Accessory::off();
}

//=======================================================
//Feuer
//Mode 82
//Type n/a
// http://www.instructables.com/id/Realistic-Fire-Effect-with-Arduino-and-LEDs/?ALLSTEPS
//=======================================================
Feuer::Feuer(uint16_t BaseAddress_, uint8_t BasePin) :
	Blinker(BaseAddress_, BasePin, 0, 0, 0, 0, 82),
	LED2(BasePin + 1, PWM_Set_On, 0, 0),
	LED3(BasePin + 2, PWM_Set_On, 0, 0) {
}

Feuer::~Feuer() {
	LED2.~LEDFader();
	LED3.~LEDFader();
}

void Feuer::SetMaxBrightness(uint16_t MaxBrightness) {
	LED1.SetMaxBrightness(MaxBrightness);
	LED2.SetMaxBrightness(MaxBrightness);
	LED3.SetMaxBrightness(MaxBrightness);
}

void Feuer::process() {
	LED1.process();
	LED2.process();
	LED3.process();

	// do nothing if not active and not is running
	if (IsActive) {
		// if time to do something, do it
		if (BlinkTimer.repeat()) {

			randomSeed(esp_random());

			// delay bestimmen
			BlinkTimer.set(random(minRandomTime, maxRandomTime));

			// Helligkeit des Feuers bestimmen
			LED1.SetBrightness(random(256));  // gelb
			LED2.SetBrightness(random(256));  // rot
			LED3.SetBrightness(random(256));  // gelb
		};
	}
	Accessory::process();
};

void Feuer::on() {
	Serial.println("Feuer::on");
	Accessory::on();
};

void Feuer::off() {
	Serial.println("Feuer::off");
	Blinker::off();
	LED1.off();
	LED2.off();
	LED3.off();
	Accessory::off();
};

//=======================================================
//Blitzlicht
//Mode 83
//Type n/a
// 
//=======================================================


Blitzlicht::Blitzlicht(uint16_t BaseAddress_, uint8_t BasePin) :
	Blinker(BaseAddress_, BasePin, 50, 20, 83),
	Status2(false),
	sleeptimeMin(5000),
	sleeptimeMax(10000),
	blitztimeMin(200),
	blitztimeMax(500) {
}

Blitzlicht::Blitzlicht(uint16_t BaseAddress_, uint8_t BasePin, uint32_t sleeptimeMin_, uint32_t sleeptimeMax_, uint16_t blitztimeMin_, uint16_t blitztimeMax_) :
	Blinker(BaseAddress_, BasePin, 50, 20, 83),
	Status2(false),
	sleeptimeMin(sleeptimeMin_),
	sleeptimeMax(sleeptimeMax_),
	blitztimeMin(blitztimeMin_),
	blitztimeMax(blitztimeMax_) {
}

Blitzlicht::~Blitzlicht() {
}

void Blitzlicht::SetMaxBrightness(uint16_t MaxBrightness) {
	LED1.SetMaxBrightness(MaxBrightness);
}

void Blitzlicht::process() {
	LED1.process();

	// do nothing if not active
	if (IsActive) {

		if (!Blitztimer.done()) {
			if (Status2) {
				Blinker::process();
			}
			else {
				LED1.off();
			}
		}
		else {
			randomSeed(esp_random());
			if (Status2) {
				Blitztimer.start(random(sleeptimeMin, sleeptimeMax)); // sleep time
			}
			else {
				Blitztimer.start(random(blitztimeMin, blitztimeMax)); // Blitztime
			}
			Status2 = !Status2;
		}
	}
	Accessory::process();
}

void Blitzlicht::on() {
	Serial.println(" Blitzlicht::on");
	Blinker::on();
	Status2 = true;
	randomSeed(esp_random());
	Blitztimer.start(random(blitztimeMin, blitztimeMax));
	Accessory::on();
}

void Blitzlicht::off() {
	Serial.println(" Blitzlicht::off");
	IsActive = false;
	Status2 = false;
	LED1.off();
	Blitztimer.stop();
	Accessory::off();
}

