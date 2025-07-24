// pinmapping.h

#ifndef _PINMAPPING_h
#define _PINMAPPING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

static uint8_t ChannelToGPIOMapping[8] = {
	17, // O11
	16, // O12
	15, // O13
	14, // O14
	10, // O15
	9,  // O16
	8,  // O17
	4  // O18
};

static uint8_t ServoChannelToGPIOMapping[3] = {
	2, // O21
	42, // O22
	41, // O23
};

#define INPUT1        21
#define INPUT2        38

#define SD_CS         10 
#define SPI_MOSI      11
#define SPI_MISO      13
#define SPI_SCK       12
#define I2S_DOUT       7
#define I2S_BCLK       5
#define I2S_LRCLK      6
#define LED_BUILTIN   47

#define DCC_INPUT     40
#define CONFIG_PIN    39

#endif

