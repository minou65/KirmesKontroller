#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#include "LEDControl.h"
#include "pinmapping.h"

LEDaccessories::LEDaccessories(uint16_t BaseAddress, byte BaseChannel) :
	Accessory(BaseAddress, BaseChannel) {
}

LEDaccessories::LEDaccessories(uint16_t BaseAddress, byte BaseChannel, byte Mode) :
	Accessory(BaseAddress, BaseChannel, Mode) {
}

LEDaccessories::~LEDaccessories() {
}
AccessoryType LEDaccessories::getType() const {
	Serial.println("LEDaccessories::getType");
	return AccessoryType::LED;
}

void LEDaccessories::SetMaxBrightness(uint16_t MaxBrightness) {
}

// ===========================================
// LED
// ===========================================
LED::LED(const uint8_t Channel) :
	_Channel(Channel) {

	Serial.println("LED::LED");

	_PWMResolution = 8;
	_PWMFrequency = 5000;

	_GPIO = ChannelToGPIOMapping[_Channel];
	Serial.print("    Channel: "); Serial.println(_Channel);
	Serial.print("    GPIO   : "); Serial.println(_GPIO);

	ledcAttachChannel(_Channel, _GPIO, _PWMFrequency, _PWMResolution);

	off();
}

LED::LED(const uint8_t Channel, uint8_t Brightness) :
	LED(Channel) {
	
	SetMaxBrightness(Brightness);
}

LED::~LED() {
	ledcDetach(_GPIO);
}

void LED::process() {}

void LED::SetMaxBrightness(uint16_t MaxBrightness) {
	Serial.println("LED::SetMaxBrightness");
	Serial.print("    MaxBrightness: "); Serial.println(MaxBrightness);
	_MaxBrightness = MaxBrightness;
}

void LED::on() {
	_IsActive = true;
	ledcWrite(_Channel, _MaxBrightness);
}

void LED::off() {
	_IsActive = false;
	ledcWrite(_Channel, PWM_Set_Off);
}

bool LED::isOn(){
	return _IsActive;
}

// ===========================================
// LEDFader
// ===========================================
LEDFader::LEDFader(const uint8_t Channel_) :
	_fadeUpIntervall(10),
	_fadeDownIntervall(10),
	_CurrentBrightness(PWM_Set_Off),
	_TargetBrightness(PWM_Set_Off),
	LED(Channel_, PWM_Set_On) {
	
	SetFadeMultiplikator(1);
	SetFadeTime(500, 500, _fadeUpIntervall, _fadeDownIntervall);

	off();
}

LEDFader::~LEDFader() {
	_fadeUpTimer.~Neotimer();
	_fadeDownTimer.~Neotimer();
}

LEDFader::LEDFader(const uint8_t Channel, uint8_t Brightness, uint16_t fadeUpTime, uint16_t fadeDownTime) :
	_fadeUpIntervall(10),
	_fadeDownIntervall(10),
	_CurrentBrightness(PWM_Set_Off),
	_TargetBrightness(PWM_Set_Off),
	LED(Channel, Brightness) {

	SetFadeMultiplikator(1);
	SetFadeTime(fadeUpTime, fadeDownTime, _fadeUpIntervall, _fadeDownIntervall);

	off();
}

// Muss regelmässig aufgerufen werden
void LEDFader::process() {

	LED::process();

	int16_t direction_;

	if (_CurrentBrightness != _TargetBrightness) {
		direction_ = _TargetBrightness - _CurrentBrightness;

		// Fade up
		if (direction_ > 0) {

			if (_fadeUpRate == 0) {
				// No fading, set direct
				_CurrentBrightness = _TargetBrightness;

			} else if (_fadeUpTimer.repeat()) {
				// fading
				_CurrentBrightness = _CurrentBrightness + _fadeUpRate;
				if (_CurrentBrightness > _TargetBrightness) {
					_CurrentBrightness = _TargetBrightness;
				}
			}
		}
		// Fade down
		else if (direction_ < 0) {

			if (_fadeDownRate == 0) {
				// Set direct the Brightness
				_CurrentBrightness = _TargetBrightness;

			} else if (_fadeDownTimer.repeat()) {
				// fading
				_CurrentBrightness = _CurrentBrightness - _fadeDownRate;

				if (_CurrentBrightness < _TargetBrightness) {
					_CurrentBrightness = _TargetBrightness;
				}
			}
		}
	}

	ledcWrite(_Channel, _CurrentBrightness);
}

void LEDFader::on() {
	_IsActive = true;
	_TargetBrightness = _MaxBrightness;
}

void LEDFader::on(uint8_t Brightness) {
	_IsActive = true;
	SetBrightness(Brightness);
};

void LEDFader::off() {
	_IsActive = false;
	_TargetBrightness = PWM_Set_Off;
}

// Brightness in Prozent 0 - 100; , wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
void LEDFader::SetPercent(uint8_t Percent, bool Hardset) {
	_IsActive = true;
	SetBrightness(((uint32_t)Percent * 255) / 100, Hardset);
}

// Brightness von 0 - 255, wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
void LEDFader::SetBrightness(uint16_t Brightness, bool Hardset) {
	if (PWM_Set_On == 0) {
		Brightness = 255 - Brightness;

		if (Brightness > PWM_Set_Off) {
			_TargetBrightness = PWM_Set_Off;
		}
		else if (Brightness < PWM_Set_On) {
			_TargetBrightness = PWM_Set_On;
		} else {
			_TargetBrightness = Brightness;
		}
	}
	else {
		if (Brightness >= _MaxBrightness)
			_TargetBrightness = _MaxBrightness;
		else
			_TargetBrightness = Brightness;
	}

	if (_IsActive && Hardset) {
		_CurrentBrightness = _TargetBrightness;
	}
}

// Setzen der maximalen Helligkeit
void LEDFader::SetMaxBrightness(uint16_t MaxBrightness) {
	if (PWM_Set_On == 0) {
		MaxBrightness = 255 - MaxBrightness;
		LED::SetMaxBrightness(MaxBrightness);
	}
	else {
		LED::SetMaxBrightness(MaxBrightness);
	}

	if (_TargetBrightness != 0)
		_TargetBrightness = _MaxBrightness;
}

// Time in ms
void LEDFader::SetFadeTime(uint16_t fadeUpTime, uint16_t fadeDownTime) {
	_fadeUpTimer.start(fadeUpTime);
	_fadeDownTimer.start(fadeDownTime);
}

// Time in ms, zudem kann das Intevall angepasst werden, kleinst möglicher Wert ist 10ms
// maximale Schritte sind 255, je mehr Schritte umso flüssiger ist das fading
// durch erhöhen von fadeIntervall können längere Zeiten erreicht werden
void LEDFader::SetFadeTime(uint16_t fadeUpTime, uint16_t fadeDownTime, uint16_t fadeUpIntervall, uint16_t fadeDownIntervall) {
	uint16_t  fadeAmount_;
	Serial.println("LEDFader::SetFadeTime");
	Serial.print("    fadeUpTime_       : "); Serial.println(fadeUpTime);
	Serial.print("    fadeDownTime_     : "); Serial.println(fadeDownTime);
	Serial.print("    fadeUpIntervall_  : "); Serial.println(fadeUpIntervall);
	Serial.print("    fadeDownIntervall_: "); Serial.println(fadeDownIntervall);

	fadeAmount_ = 0;
	if (fadeUpIntervall < 10) fadeUpIntervall = 10;
	if (fadeUpIntervall < 10) fadeUpIntervall = 10;

	if (fadeUpTime > 0) {
		int denominator_ = fadeUpTime / fadeUpIntervall;
		if (denominator_ != 0)
			fadeAmount_ = 255 / denominator_;
	}
	_fadeUpRate = fadeAmount_;

	fadeAmount_ = 0;
	if (fadeDownTime > 0) {
		int denominator_ = fadeDownTime / fadeDownIntervall;
		if (denominator_ != 0)
			fadeAmount_ = 255 / denominator_;
	}
	_fadeDownRate = fadeAmount_;

	SetFadeTime(fadeUpTime, fadeDownTime);

}

// Mit diser funktion kann das Intervall zusätzlich erhöht werden. Sollte nicht mehr verwendet werden
void LEDFader::SetFadeMultiplikator(uint8_t Multiplikator) {
	_FadeMultiplikator = Multiplikator;
}

// Liefert die aktuelle helligkeit
uint8_t LEDFader::GetCurrentBrightness() {
	uint8_t result_ = 255 - _CurrentBrightness;

	if (PWM_Set_On == 0) {
		if (result_ < PWM_Set_On) {
			result_ = PWM_Set_On;
		}
	}
	else {
		result_ =  _CurrentBrightness;
	}
	return result_;
}

// Prüft ob die LED ganz Dunkel ist
bool LEDFader::isDark() {
	return _CurrentBrightness == PWM_Set_Off;
}

// ===========================================
// NatriumLamp
// ===========================================
Natrium::Natrium(const uint8_t Channel, const bool  IsMalFunction, uint8_t fadeOnIntervall, uint8_t fadeOffIntervall) :
	LEDFader(Channel, PWM_Set_On, 0, 0),
	_IsMalFunction(IsMalFunction) {

	SetFadeTime(fadeOnIntervall, fadeOffIntervall);
	off(true);
};

Natrium::~Natrium() {
	_Malfunctiontimer.~Neotimer();
	_Operationtimer.~Neotimer();
}

void Natrium::SetMaxBrightness(uint16_t MaxBrightness) {
	LEDFader::SetMaxBrightness(MaxBrightness);
}

void Natrium::process() {

	LEDFader::process();

	if (_Operationtimer.repeat()) {

		// Lampe glüht und baut nun Betriebsdruck auf
		if (_CurrentStatus == status::GlowingOn) {
			_CurrentStatus = status::OperatingPressure;
			SetPercent(random(_randomMin, _randomMax), true);

			// Wenn defekt, dann Ausfallzeit berechnen
			if (_IsMalFunction) {
				_Malfunctiontimer.start(random(_MalFunctionIntervalMin, _MalFunctionIntervalMax));
			}

		}

		// Lampe baut Betriebsdruck auf
		else if ((_CurrentStatus == status::OperatingPressure) && (GetCurrentBrightness() != _MaxBrightness)) {
			SetBrightness(GetCurrentBrightness() + 5);
		}

		// Lampe hat Betriebsdruck erreicht
		else if ((_CurrentStatus == status::OperatingPressure) && (GetCurrentBrightness() == _MaxBrightness)) {
			_CurrentStatus = status::On;
			Serial.println("Natrium::process, Lampe hat Betriebsdruck erreicht");
		}

		// Lampe Glüht aus
		else if ((_CurrentStatus == status::GlowingOff) && (GetCurrentBrightness() != PWM_Set_Off) && _IsActive) {
			SetBrightness(GetCurrentBrightness() - 3);
		}

		// Lampe ist Ausgeglüht
		else if ((_CurrentStatus == status::GlowingOff) && (GetCurrentBrightness() == PWM_Set_Off) && _IsActive) {
			_CurrentStatus = status::Off;
		}

		// Lampe ist defekt und fällt nun aus
		else if ((_CurrentStatus == status::OperatingPressure || _CurrentStatus == status::On) && (_IsActive) && (_IsMalFunction) && (_Malfunctiontimer.repeat())) {
			// Ausschalten, Lampe glüht noch
			_CurrentStatus = status::GlowingOff;
			SetPercent(random(_randomMin, _randomMax), true);

			// Einschaltzeit berechnen
			_Malfunctiontimer.start(random(_MalFunctionIntervalMin, _MalFunctionIntervalMax));
		}

		// Lampe ist defekt und schaltet nun wieder ein
		else if ((_CurrentStatus == status::GlowingOff || _CurrentStatus == status::Off) && _IsActive && _IsMalFunction && (_Malfunctiontimer.repeat())) {
			// Vorglühen 
			_CurrentStatus = status::GlowingOn;
			SetPercent(random(_randomMin, _randomMax), true);
		}


	}
};

void Natrium::on() {
	Serial.println("Natrium::on");
	_IsActive = true;

	// Vorglühen 
	_CurrentStatus = status::GlowingOn;


	_Operationtimer.start(_OperationTime);
};

void Natrium::off() {
	Serial.println("Natrium::off");
	_IsActive = false;

	// Ausschalten, Lampe glüht noch
	_CurrentStatus = status::GlowingOff;
	SetPercent(random(_randomMin, _randomMax), true);
};

void Natrium::off(bool force_) {
	Serial.println("Natrium::off(bool force_)");
	if (force_) {
		LEDFader::off();
		_CurrentStatus = status::Off;
	}
	else {
		off();
	}
};

// ===========================================
// NeonLamp
// ===========================================
Neon::Neon(const uint8_t Channel_, const bool MalFunction_) :
	LED(Channel_),
	_IsMalFunction(MalFunction_) {
}
Neon::~Neon(){
	_Operationtimer.~Neotimer();
}

void Neon::SetMaxBrightness(uint16_t MaxBrightness){
	LED::SetMaxBrightness(MaxBrightness);
}

void Neon::process() {

	LED::process();

	// do nothing if not active and not is running
	if (!_IsActive)
		return;

	// if time to do something, do it
	if (_Operationtimer.repeat()) {

		// Wir müssen blinken
		if (_FlashCounter > 0) {

			if (_CurrentStatus) {
				// Einsachltverzögerung
				_Operationtimer.set(random(300, 500));
				// Lampe aus
				ledcWrite(_Channel, PWM_Set_Off);
			}
			else {
				// Ausschaltverzögerung
				_Operationtimer.set(random(200, 400));
				// Lampe ein
				ledcWrite(_Channel, _MaxBrightness);
			}

			// Defekte Lampe, Einschaltverzögern darf grösser sein
			if (_CurrentStatus && _IsMalFunction)
				_Operationtimer.set(random(200, 2000));

			// Blinker runterzählen wenn Lampe nicht defekt ist
			if (!_IsMalFunction)
				_FlashCounter--;

			_CurrentStatus = !_CurrentStatus;

		}
		else {
			// Wir müssen nicht mehr blinken, Lampe ein
			ledcWrite(_Channel, _MaxBrightness);
		}
	}
}

void Neon::on() {
	_IsActive = true;
	_CurrentStatus = false;

	// Blinker festlegen
	_FlashCounter = random(1, 10);

	// Defekte Lampe dann muss m_Flashcount auf jedenfall grösser als 0 sein
	if (_IsMalFunction)
		_FlashCounter = 1;

	// Wie lange soll gewartet werden bis zum ersten Einschalten
	_Operationtimer.start(random(400, 600));
}

void Neon::off() {
	LED::off();
}
