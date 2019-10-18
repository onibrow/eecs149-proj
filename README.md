# EECS149/249A Final Project - Bop It! Rhythm Game

Sanga Park, Seiya Ono, Ryan Tsang, Jehan Yang

## Proposal (10/25)

Rhythm games have always been popular - we have Dance Dance Revolution, Tap Tap Revenge, Guitar Hero, Rhythm Heaven, Elite Beat Agents, osu!, Beat Saber, Dance Rush, Simon Says, and more. In our project, we would like to tackle one of these types of rhythm games from a new angle. We will focus on the discrete note, single dimension approach to rhythm games similar to Guitar Hero or Tap Tap Revenge - these games require the player to on beat, actual inputs from a pool of predefined discrete buttons; for Guitar Hero's case, a selection of 5 frets and a strummer, and in Tap Tap Revenge's case, 4 discrete horizontle buttons. We will take this style of gameplay and translate into something no one has seen before - a Bop It! driven hardware implemented beat map.

Specifically, we will use the three actuation points on the Has🅱️ro Bop It! (Twist-It, Pull-It, and Bop-It) as inputs to our game that displays its oncoming beats not on a screen but on 3 LED strips. There will be indicators giving the user feedback on whether or not they were able to successfully hit each note, and will total up the score at the very end for high octane, competitive gameplay.


## Useful Links:

[RPi WS2801 SPI Interface](https://tutorials-raspberrypi.com/how-to-control-a-raspberry-pi-ws2801-rgb-led-strip/)

[Adafruit WS2801 RPi SPI Library](https://github.com/adafruit/Adafruit_Python_WS2801/blob/master/Adafruit_WS2801/WS2801.py)

[RPI GPIO Interrupt Handling](http://raspberrywebserver.com/gpio/using-interrupt-driven-gpio.html)
