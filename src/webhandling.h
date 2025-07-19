// webhandling.h

#ifndef _WEBHANDLING_h
#define _WEBHANDLING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <IotWebConf.h>
#include <IotWebConfOptionalGroup.h>
#include <IotWebConfParameter.h>
#include "common.h"
#include "neotimer.h"

void setupWeb();
void loopWeb();

extern void handleDecoderGroup(uint8_t DecoderGroup);
extern bool DecoderGroupIsEnabled(uint8_t DecoderGroup);

class SoundGroup : public iotwebconf::ParameterGroup {
public:
    SoundGroup(const char* id, const char* name)
        : ParameterGroup(id, name),
        _volumeBuffer{ 0 },
        _balanceBuffer{ 0 },
        _monoBuffer{ 0 }
    {
        snprintf(_volumeId, STRING_LEN, "%s-volume", this->getId());
        snprintf(_balanceId, STRING_LEN, "%s-balance", this->getId());
        snprintf(_monoId, STRING_LEN, "%s-mono", this->getId());

        this->addItem(&_volumeParameter);
        this->addItem(&_monoParameter);
        this->addItem(&_balanceParameter);
    }

	int getVolume() const {
		return atoi(_volumeBuffer);
	}

	int getBalance() const {
		return atoi(_balanceBuffer);
	}

	bool isMono() const {
		return _monoBuffer[0] == '1';
	}

    // Volume-Parameter (0-21, Default 15)
    iotwebconf::NumberParameter _volumeParameter = iotwebconf::NumberParameter(
        "Volume",
        _volumeId,
        _volumeBuffer,
        sizeof(_volumeBuffer),
        "15",
        "0..21",
        "min='0' max='21'"
    );

    // Mono-Checkbox (Default: aus)
    iotwebconf::CheckboxParameter _monoParameter = iotwebconf::CheckboxParameter(
        "Mono",
        _monoId,
        _monoBuffer,
        sizeof(_monoBuffer),
        false // Default: nicht aktiviert
    );

    // Balance-Parameter (-16 bis +16, Default 0)
    iotwebconf::NumberParameter _balanceParameter = iotwebconf::NumberParameter(
        "Balance",
        _balanceId,
        _balanceBuffer,
        sizeof(_balanceBuffer),
        "0",
        "-16..0..16",
        "min='-16' max='16'"
    );

private:
	char _volumeBuffer[5];
	char _balanceBuffer[5];
	char _monoBuffer[2];

	char _volumeId[STRING_LEN] = "soundVolume";
	char _balanceId[STRING_LEN] = "soundBalance";
	char _monoId[STRING_LEN] = "soundMono";

};

class MySelectParameter : public iotwebconf::SelectParameter {
public:
    MySelectParameter(
        const char* label,
        const char* id,
        char* valueBuffer,
        int length,
        const char* optionValues,
        const char* optionNames,
        size_t optionCount,
        size_t nameLength,
        const char* defaultValue,
        const char* customHtml
    );
};

class AccessoryGroup : public iotwebconf::ChainedParameterGroup {
public:
    AccessoryGroup(const char* id, const char* label)
        : ChainedParameterGroup(id, label) {
        snprintf(_designationId, STRING_LEN, "%s-designation", this->getId());
        snprintf(_addressId, STRING_LEN, "%s-address", this->getId());
        snprintf(_modeCustomHTML, STRING_LEN, "onchange=\"hideClass('%s')\"", this->getId());

        // sound extension
        snprintf(_FilenameId, STRING_LEN, "%s-filename", this->getId());
        snprintf(_ActiveDurationId, STRING_LEN, "%s-activeduration", this->getId());
        snprintf(_InputPinId, STRING_LEN, "%s-inputpin", this->getId());

        this->addItem(&_designationParam);
        this->addItem(&_addressParam);

		// sound extension
        this->addItem(&_filenameParam);
        this->addItem(&_activeDurationParam);
        this->addItem(&_InputPinParam);
    }

    void applyDefaultValues() {
        _designationParam.applyDefaultValue();
        _addressParam.applyDefaultValue();

		// sound extension
        _filenameParam.applyDefaultValue();
        _activeDurationParam.applyDefaultValue();
        _InputPinParam.applyDefaultValue();

    }

    int getAddress() const {
        return atoi(_addressValue);
    }
    const char* getDesignation() const {
        return _designationValue;
    }

	// sound extension
    char* getSoundFilename() {
        return _filenameValue;
    }

    int getActiveDuration() const {
        return atoi(_activeDurationValue);
    }

    int getInputPin() const {
        return atoi(_InputPinValue);
	}

protected:
    char _designationId[STRING_LEN];
    char _addressId[STRING_LEN];
	char _modeCustomHTML[STRING_LEN];

    String getEndTemplate() override {
        String result_ = "<script>hideClass('%s')</script>\n";
        result_.replace("%s", this->getId());
        result_ += ChainedParameterGroup::getEndTemplate();
        return result_;
    };

    char _designationValue[STRING_LEN];
    char _addressValue[NUMBER_LEN];

    iotwebconf::TextParameter _designationParam =
        iotwebconf::TextParameter("Designation", _designationId, _designationValue, STRING_LEN);

    iotwebconf::NumberParameter _addressParam =
        iotwebconf::NumberParameter("DCC Address", _addressId, _addressValue, NUMBER_LEN, "3", "1..1024", "min='1' max='1024' step='1'");

    // Sound extension
    char _filenameValue[STRING_LEN];
    char _activeDurationValue[NUMBER_LEN];
    char _InputPinValue[NUMBER_LEN] = "0";

    char _FilenameId[STRING_LEN];
    char _ActiveDurationId[STRING_LEN];
    char _InputPinId[STRING_LEN];

    Neotimer _timer = Neotimer();

    iotwebconf::TextParameter _filenameParam =
        iotwebconf::TextParameter("Filename", _FilenameId, _filenameValue, STRING_LEN);

    iotwebconf::NumberParameter _activeDurationParam =
        iotwebconf::NumberParameter("Active Duration (ms)", _ActiveDurationId, _activeDurationValue, NUMBER_LEN, "1000", "1..60000", "min='1' max='60000' step='1'");

    MySelectParameter _InputPinParam =
        MySelectParameter(
            "Input Pin",
            _InputPinId,
            _InputPinValue,
            NUMBER_LEN,
            (char*)InputPinValues,
            (char*)InputPinNames,
            sizeof(InputPinValues) / NUMBER_LEN,
            STRING_LEN,
            "0",
            nullptr
        );
};

class ServoGroup : public AccessoryGroup {
public:
    ServoGroup(const char* id) : AccessoryGroup(id, "Servo") {

		sniprintf(_ModeId, STRING_LEN, "%s-mode", this->getId());
        snprintf(_travelTimeId, STRING_LEN, "%s-traveltime", this->getId());
        snprintf(_multiplierId, STRING_LEN, "%s-multiplier", this->getId());
        snprintf(_limit1Id, STRING_LEN, "%s-limit1", this->getId());
        snprintf(_limit2Id, STRING_LEN, "%s-limit2", this->getId());
        snprintf(_TimeOnId, STRING_LEN, "%s-timeon", this->getId());
        snprintf(_TimeOffId, STRING_LEN, "%s-timeoff", this->getId());
        
		this->addItem(&_ModeParam);
        this->addItem(&this->_travelTimeParam);
        this->addItem(&this->_multiplierParam);
        this->addItem(&this->_limit1Param);
        this->addItem(&this->_limit2Param);
        this->addItem(&_TimeOnParam);
        this->addItem(&_TimeOffParam);
    }

    void applyDefaultValues() {
		AccessoryGroup::applyDefaultValues();
        _travelTimeParam.applyDefaultValue();
        _multiplierParam.applyDefaultValue();
        _limit1Param.applyDefaultValue();
        _limit2Param.applyDefaultValue();
		_ModeParam.applyDefaultValue();
    }

    uint8_t getMode() const {
        return atoi(_ModeValue);
	}

    int getTravelTime() const {
        return atoi(_travelTimeValue);
    }
    int getMultiplier() const {
        return atoi(_multiplierValue);
    }
    int getLimit1() const {
        return atoi(_limit1Value);
    }
    int getLimit2() const {
        return atoi(_limit2Value);
    }
    int getTimeOn() const {
        return atoi(_TimeOnValue);
    }
    int getTimeOff() const {
        return atoi(_TimeOffValue);
	}

private:
    char _travelTimeId[STRING_LEN];
    char _multiplierId[STRING_LEN];
    char _limit1Id[STRING_LEN];
    char _limit2Id[STRING_LEN];
	char _ModeId[STRING_LEN];
    char _TimeOnId[STRING_LEN];
    char _TimeOffId[STRING_LEN];

    char _travelTimeValue[NUMBER_LEN];
    char _multiplierValue[NUMBER_LEN];
    char _limit1Value[NUMBER_LEN];
    char _limit2Value[NUMBER_LEN];
    char _ModeValue[STRING_LEN];
    char _TimeOnValue[NUMBER_LEN];
    char _TimeOffValue[NUMBER_LEN];

    iotwebconf::NumberParameter _travelTimeParam =
        iotwebconf::NumberParameter("Travel time (ms)", _travelTimeId, _travelTimeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter _multiplierParam =
        iotwebconf::NumberParameter("Multiplier", _multiplierId, _multiplierValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter _limit1Param =
        iotwebconf::NumberParameter("Limit 1", _limit1Id, _limit1Value, NUMBER_LEN, "0", "0..180", "min='0' max='180' step='1'");

    iotwebconf::NumberParameter _limit2Param =
        iotwebconf::NumberParameter("Limit 2", _limit2Id, _limit2Value, NUMBER_LEN, "180", "1..180", "min='0' max='180' step='1'");

    iotwebconf::NumberParameter _TimeOnParam =
        iotwebconf::NumberParameter("Time On (ms)", _TimeOnId, _TimeOnValue, NUMBER_LEN, "1000", "1..99999", "min='1' max='99999' step='1'");

    iotwebconf::NumberParameter _TimeOffParam =
        iotwebconf::NumberParameter("Time Off (ms)", _TimeOffId, _TimeOffValue, NUMBER_LEN, "1000", "1..99999", "min='1' max='99999' step='1'");

    MySelectParameter _ModeParam =
        MySelectParameter(
            "Mode",
            _ModeId,
            _ModeValue,
            STRING_LEN,
            (char*)ServoModeValues,
            (char*)ServoModeNames,
            sizeof(ServoModeValues) / STRING_LEN,
            STRING_LEN,
            nullptr,
            _modeCustomHTML
        );

};

class OutputGroup : public AccessoryGroup {
public:
    OutputGroup(const char* id) : AccessoryGroup(id, "Output") {
        snprintf(_ModeId, STRING_LEN, "%s-mode", this->getId());
        snprintf(_NumberId, STRING_LEN, "%s-number", this->getId());
        snprintf(_TimeOnId, STRING_LEN, "%s-timeon", this->getId());
        snprintf(_TimeOffId, STRING_LEN, "%s-timeoff", this->getId());
        snprintf(_MultiplierId, STRING_LEN, "%s-multiplier", this->getId());
        snprintf(_TimeOnFadeId, STRING_LEN, "%s-onfade", this->getId());
        snprintf(_TimeOffFadeId, STRING_LEN, "%s-offfade", this->getId());
        snprintf(_BrightnessId, STRING_LEN, "%s-brightness", this->getId());


        this->addItem(&_ModeParam);
        this->addItem(&_NumberParam);
        this->addItem(&_TimeOnParam);
        this->addItem(&_TimeOffParam);
        this->addItem(&_MultiplierParam);
        this->addItem(&_TimeOnFadeParam);
        this->addItem(&_TimeOffFadeParam);
        this->addItem(&_BrightnessParam);

    }

    char _ModeValue[STRING_LEN];
    char _NumberValue[NUMBER_LEN];
    char _TimeOnValue[NUMBER_LEN];
    char _TimeOffValue[NUMBER_LEN];
    char _MultiplierValue[NUMBER_LEN];
    char _TimeOnFadeValue[NUMBER_LEN];
    char _TimeOffFadeValue[NUMBER_LEN];
    char _BrightnessValue[NUMBER_LEN] = "255";

    MySelectParameter _ModeParam =
        MySelectParameter(
            "Mode",
            _ModeId,
            _ModeValue,
            STRING_LEN,
            (char*)OutputModeValues,
            (char*)OutputModeNames,
            sizeof(OutputModeValues) / STRING_LEN,
            STRING_LEN,
            nullptr,
            _modeCustomHTML
        );

    iotwebconf::NumberParameter _NumberParam =
        iotwebconf::NumberParameter("Number of outputs", _NumberId, _NumberValue, NUMBER_LEN, "1", "1..8", "min='1' max=8' step='1'");

    iotwebconf::NumberParameter _TimeOnParam =
        iotwebconf::NumberParameter("Time On (ms)", _TimeOnId, _TimeOnValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter _TimeOffParam =
        iotwebconf::NumberParameter("Time Off (ms)", _TimeOffId, _TimeOffValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter _MultiplierParam =
        iotwebconf::NumberParameter("Multiplier", _MultiplierId, _MultiplierValue, NUMBER_LEN, "10", "1..255", "min='0' max='255' step='1'");

    iotwebconf::NumberParameter _TimeOnFadeParam =
        iotwebconf::NumberParameter("Time On Fade (ms)", _TimeOnFadeId, _TimeOnFadeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter _TimeOffFadeParam =
        iotwebconf::NumberParameter("Time Off Fade (ms)", _TimeOffFadeId, _TimeOffFadeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter _BrightnessParam =
        iotwebconf::NumberParameter("Brightness", _BrightnessId, _BrightnessValue, NUMBER_LEN, "255", "0..255", "min='0' max='255' step='1'");

    int getBrightness() const {
        return atoi(_BrightnessValue);
    }

    int getNumber() const {
        return atoi(_NumberValue);
    }

    void applyDefaultValues() {
        AccessoryGroup::applyDefaultValues();
        _ModeParam.applyDefaultValue();
        _NumberParam.applyDefaultValue();
        _TimeOnParam.applyDefaultValue();
        _TimeOffParam.applyDefaultValue();
        _MultiplierParam.applyDefaultValue();
        _TimeOnFadeParam.applyDefaultValue();
        _TimeOffFadeParam.applyDefaultValue();
        _BrightnessParam.applyDefaultValue();
        _filenameParam.applyDefaultValue();
        _activeDurationParam.applyDefaultValue();
        _InputPinParam.applyDefaultValue();
    }

    int getMode() const {
        return atoi(_ModeValue);
	}

private:
    char _ModeId[STRING_LEN];
    char _NumberId[STRING_LEN];
    char _TimeOnId[STRING_LEN];
    char _TimeOffId[STRING_LEN];
    char _MultiplierId[STRING_LEN];
    char _TimeOnFadeId[STRING_LEN];
    char _TimeOffFadeId[STRING_LEN];
    char _BrightnessId[STRING_LEN];
};

extern SoundGroup soundGroup;
extern ServoGroup ServoGroup1;
extern ServoGroup ServoGroup2;
extern ServoGroup ServoGroup3;
extern OutputGroup OutputGroup1;
extern OutputGroup OutputGroup2;
extern OutputGroup OutputGroup3;
extern OutputGroup OutputGroup4;
extern OutputGroup OutputGroup5;
extern OutputGroup OutputGroup6;
extern OutputGroup OutputGroup7;
extern OutputGroup OutputGroup8;

#endif

