# Kirmes kontroller

## Controller
[YB-ESP32-S3-AMP](https://github.com/yellobyte/YB-ESP32-S3-AMP)

## Librarys
 - [DFRobot DFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini)

# Pin mapping

```c++
#define I2S_DOUT       7  // DIN (digital audio signal)
#define I2S_BCLK       5  // BCLK (bit clock)
#define I2S_LRCLK      6  // LRCLK (frame clock)

#define SD_CS         10 // SCS (SPI bus control, chip select, this control line is not needed for SD_MMC-lib and available for other usage when solder bridge SD_CS is open [default closed])
#define SPI_MOSI      11 // MOSI (SPI bus data communication, SD_MMC calls it CMD)
#define SPI_MISO      13 // SCK (SPI bus clock signal)
#define SPI_SCK       12 // MISO (SPI bus data communication, SD_MMC calls it D0)

#define LED_BUILTIN   47

```

```
static uint8_t ChannelToGPIOMapping[16] = {
	GPIO_NUM_17, // O11
	GPIO_NUM_16, // O12
	GPIO_NUM_15, // O13
	GPIO_NUM_14, // O14
	GPIO_NUM_10, // O15
	GPIO_NUM_9,  // O16
	GPIO_NUM_8,  // O17
	GPIO_NUM_4,  // O18

	GPIO_NUM_42, // JP1
	GPIO_NUM_41, // JP2
	GPIO_NUM_40, // JP3

	GPIO_NUM_47, // Input 1 (JP4/2)
	GPIO_NUM_48, // Input 2 (JP4/3)
};
```

## Settings
```cpp
#define BUTTON_PIN 2 // Pin 2 for the button
#define MOTOR_PIN 3 // Pin 3 for the motor output
#define SOUND_INDEX 1 // First sound (DFPlayer starts at 1)
#define SOUND_VOLUME 10 // Volume level (0-30)
#define PLAY_DURATION 60000 // 60 seconds in milliseconds
```

## Kirmes sounds
https://www.pond5.com/de/search?kw=karussell-unterhaltung-park-kirmes&media=sfx