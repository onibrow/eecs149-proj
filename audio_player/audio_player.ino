
// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define BREAKOUT_RESET  7 // 9      // VS1053 reset pin (output)                   // PB1 DNE -> GPIO 5 = Pin 7
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)             // PB2 Ex = SS       = Pin 10
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)     // PB0 Ex = GPIO 6   = Pin 8
//#define SHIELD_CS     7      // VS1053 chip select pin (output)
//#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 6 // 4     // Card chip select pin                                  // PD4 DNE -> GPIO 4 = Pin 6
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 2 // 3       // VS1053 Data request, ideally an Interrupt pin         // PD3 DNE -> GPIO 3 = Pin 5

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

int buttonOneState = 0;         // current state of the button
int lastButtonOneState = 0;     // previous state of the button

int buttonTwoState = 0;
int lastButtonTwoState = 0;

int audio_a = 17;
int audio_b = 16; 

void setup() {
  Serial.begin(9600);
  delay(3000);
  pinMode(audio_a, INPUT);
  pinMode(audio_b, INPUT);

  if (! musicPlayer.begin()) {
    while (1);
  }

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  SD.open("/");

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}

void loop() {
  // File is playing in the background
  buttonOneState = digitalRead(audio_a);
  delay(1);

  bool buttonOnePosTrigger = false;
  bool buttonTwoPosTrigger = false;

  if (buttonOneState == HIGH && buttonOneState != lastButtonOneState) {
    buttonOnePosTrigger = true;
  }
  lastButtonOneState = buttonOneState;

  buttonTwoState = digitalRead(audio_b);
  if (buttonTwoState == HIGH && buttonTwoState != lastButtonTwoState) {
    buttonTwoPosTrigger = true;
  }
  lastButtonTwoState = buttonTwoState;

  if (!musicPlayer.stopped() && (buttonOnePosTrigger || buttonTwoPosTrigger)) {
    musicPlayer.stopPlaying();
  } else {
    if (buttonOnePosTrigger && buttonTwoPosTrigger) {
      musicPlayer.startPlayingFile("groovin.mp3");
    } else if (buttonOnePosTrigger) {
      musicPlayer.startPlayingFile("sugar.mp3");
    } else if (buttonTwoPosTrigger) {
      musicPlayer.startPlayingFile("track001.mp3");
    }
  }

  delay(50);
}
