// common.h

#ifndef _COMMON_h
#define _COMMON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "pinmapping.h"

#define STRING_LEN 30
#define NUMBER_LEN 6

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN GPIO_NUM_39

extern bool ResetDCCDecoder;
extern char Version[];

static char OutputModeValues[][STRING_LEN] = {
    "0",
    "1",
    "40",
    "50",
    "51",

    "80",
    "81",
    "82",
    "83",

    "52",
    "53",
    "54",
    "55",

    "60",
    "61",
    "62",

    //"102",
    //"103",
    //"104",
    //"105",
    //"106",
    //"110",

    //"70",

    "201",
    "202"

    "203"
};

static char OutputModeNames[][STRING_LEN] = {
    "-",                    // 0
    "Watchdog",             // 1
    "einfacher Ausgang (1)",// 40
    "Blinker (1)",          // 50
    "Wechselblinker (2)",   // 51

    "Fernseher (1)",        // 80
    "Schweisslicht (3)",    // 81
    "Feuer (3)",            // 82
    "Blitzlicht (1)",       // 83

    "Lauflicht 1 (1..16)", // 52
    "Lauflicht 2 (1..16)", // 53
    "Lauflicht 3 (1..16)", // 54
    "Lauflicht 4 (1..16)", // 55

    "Hausbeleuchtung (1..16)", // 60
    "Neonlampen (1..16)",      // 61
    "Natriumlampen (1..16)",   // 62
     
    //"SBB_Hauptsignal_102 (2)", // 102
    //"SBB_Hauptsignal_103 (3)", // 103
    //"SBB_Hauptsignal_104 (4)", // 104
    //"SBB_Hauptsignal_105 (5)", // 105
    //"SBB_Hauptsignal_106 (5)", // 106
    //"SBB_Zwergsignal_110 (3)", // 110
     
    //"Strassen_Signal_70 (3)",  // 70
     
    "Entkuppler (1)",          // 201
    "Weiche (2)",              // 202

    "Motor (1)"              // 203
};

static char ServoModeValues[][STRING_LEN] = {
    "250",
    "251",
    "252",
    "253"
};

static char ServoModeNames[][STRING_LEN] = {
    "-",           // 0
    "Impuls",      // 251, nach einer eingestellten Zeit geht wieder zurück in die Ausgangsposition
    "Flip",        // 252, wechselt zwischen zwei Positionen
    "Pendel"       // 253, wechsselt zwischen den positionen bis der Servo ausgeschaltet wird
};

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

static const char InputPinValues[][NUMBER_LEN] = { "0", STR(INPUT1), STR(INPUT2) };
static const char InputPinNames[][STRING_LEN] = { "-", "Input1", "Input2" };

#endif

