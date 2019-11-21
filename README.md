# EECS149/249A Final Project - Bop It! Rhythm Game

Sanga Park, Seiya Ono, Ryan Tsang, Jehan Yang

Setting up a lab computer:

```bash
cd ~/buckler/software
git clone https://github.com/onibrow/eecs149-proj
cd eecs149-proj
./setup_vm
```

## Proposal (10/25)

[Spreadsheet](https://docs.google.com/spreadsheets/d/1WutyDuHFwCbV_97uEuxx-2Y0jQ0Z9K7RMsq6U2ZCgrc/edit?usp=sharing)

Rhythm games have always been popular - we have Dance Dance Revolution, Tap Tap Revenge, Guitar Hero, Rhythm Heaven, Elite Beat Agents, osu!, Beat Saber, Dance Rush, Simon Says, and more. In our project, we would like to tackle one of these types of rhythm games from a new angle. We will focus on the discrete note, single dimension approach to rhythm games similar to Guitar Hero or Tap Tap Revenge - these games require the player to on beat, actual inputs from a pool of predefined discrete buttons; for Guitar Hero's case, a selection of 5 frets and a strummer, and in Tap Tap Revenge's case, 4 discrete horizontle buttons. We will take this style of gameplay and translate into something no one has seen before - a Bop It! driven hardware implemented beat map.

Specifically, we will use the three actuation points on the Has🅱️ro Bop It! (Twist-It, Pull-It, and Bop-It) as inputs to our game that displays its oncoming beats not on a screen but on 3 LED strips. There will be indicators giving the user feedback on whether or not they were able to successfully hit each note, and will total up the score at the very end for high octane, competitive gameplay.

## Getting out of dependency hell

1. Try to compile. What library is it missing? ex: `blah.h`
1. Does the file exist in the sdk? Find it by going to the sdk folder `~/buckler/software/nrf52x-base/sdk/nrf5_sdk_15.3.0` and running `find . -name 'blah.*'`. Take note of the directory the header and C file are located in.
1. Edit `eecs149-proj/vm_setup/Includes.mk`. You will need to add **2 Lines**:
    1. Near line 180, the `SDK_HEADER_PATHS` should end and become `SDK_SOURCE_PATHS`. Add a new line at the end of the header paths and add the directory you found in step 2 for the header file. You might want to check if that directory is being checked already. ex: `SDK_HEADER_PATHS += $(SDK_ROOT)external/components/`
    1. Near line 270, the `SDK_SOURCE_PATHS` should end and is followed by `ifdef SERIALIZATiON_MODE`. Add a new line at the end of the source paths and add the directory you found in step 2 for the C file. Similarly, you might want to check if that directory is already being sourced. ex: `SDK_SOURCE_PATHS += $(SDK_ROOT)external/components/`
1. Edit `eecs149-proj/vm_setup/Board.mk`. You will need to add **1 Line**:
    1. Near line 95, the list of `BOARD_SOURCES` should end. Add the C file at the end of the list (make sure to terminate with a `\`). ex: `blah.c\`
1. Repeat the above steps for all dependencies.

## Meeting Notes

### 11/7

MP3 files are compressed so they need to be decoded to be played on a DAC -> this isn't particularly ideal so we will switch over to WAV files which are uncompressed. From Audacity, we can see that we can save them in various formats (16 bit or 24 bit PCM), which will allow us to basically read in binary and shove them into either the I2S registers or over I2C. [Here](https://www.sparkfun.com/tutorials/160)'s a tutorial on how to play WAV files and goes over their limitations. 

## Useful Links:

[nRF52 Product Spec](https://infocenter.nordicsemi.com/pdf/nRF52832_PS_v1.4.pdf)

* I2S begins page 445
* [Loopback example for sanity check](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.2.0%2Fi2s_example_loopback.html&cp=4_0_0_4_5_14)
* [nRF SDK I2S](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/group__nrfx__i2s.html)
* [MCP4725 DAC Driver](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/group__mcp4725.html)

[Nordic SD Card](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/app_sdcard_example.html)

[Nordic Timers](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.3.0%2Fgroup__app__timer.html)

[SparkFun's Intro to SPI](https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi/all)

[RPi WS2801 SPI Interface](https://tutorials-raspberrypi.com/how-to-control-a-raspberry-pi-ws2801-rgb-led-strip/)

[WS2801 Datasheet](https://cdn-shop.adafruit.com/datasheets/WS2801.pdf)

[Adafruit WS2801 RPi SPI Library](https://github.com/adafruit/Adafruit_CircuitPython_WS2801)

[Audio Python Syncing](https://github.com/scottlawsonbc/audio-reactive-led-strip)

[RPI GPIO Interrupt Handling](http://raspberrywebserver.com/gpio/using-interrupt-driven-gpio.html)
