// common.h

#ifndef _COMMON_h
#define _COMMON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define STRING_LEN 30
#define NUMBER_LEN 5

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN GPIO_NUM_39

extern bool ResetDCCDecoder;
extern char Version[];

static char DecoderModeValues[][STRING_LEN] = {
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
    "102",
    "103",
    "104",
    "105",
    "106",
    "110",
    "70",
    "201",
    "202"
};

static char DecoderModeNames[][STRING_LEN] = {
    "-",
    "Watchdog",
    "einfacher Ausgang (1)",  // ok
    "Blinker (1)",  // ok, fading ok
    "Wechselblinker (2)", // ok
    "Fernseher (1)",  // 80 ok
    "Schweisslicht (3)", // 81 ok
    "Feuer (3)", // 82 ok
    "Blitzlicht (1)", // 83 ok   
    "Lauflicht 1 (1..16)", // ok
    "Lauflicht 2 (1..16)",  // ok
    "Lauflicht 3 (1..16)",  // ok
    "Lauflicht 4 (1..16)",  // ok

    "Hausbeleuchtung (1..16)", // ok
    "Neonlampen (1..16)", // ok
    "Natriumlampen (1..16)", // ok
    "SBB_Hauptsignal_102 (2)",
    "SBB_Hauptsignal_103 (3)",
    "SBB_Hauptsignal_104 (4)",
    "SBB_Hauptsignal_105 (5)",
    "SBB_Hauptsignal_106 (5)",
    "SBB_Zwergsignal_110 (3)",
    "Strassen_Signal_70 (3)",
    "Entkuppler (1)", // ok
    "Weiche (2)",  // ok
};

#endif

