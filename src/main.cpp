#include <Arduino.h>
#include <FastLED.h>
#include <SoftwareSerial.h>

#define TRIGGER 15

#define DATA_PIN    16
#define NUM_LEDS    30
#define BRIGHTNESS  255
#define GRADIENT_PALLETE_END 200
#define LED_TYPE    WS2812B

CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE( royal_BTe_gd ) {
  0,     0,  0,  0,   //black
255,   0,  0,  80}; // CRGB::RoyalBTe

CRGBPalette16 myPal = royal_BTe_gd;
SoftwareSerial BT(8, 7);

int first_clap_millis = 0;
int claps = 0;

void switch_leds();

uint8_t timeDelay = 5;  // time in ms.  Lower number is faster travel.

void setup() {
  //Serial.begin(9600);
  //BT.begin(9600);
  pinMode(TRIGGER, INPUT);

  // LED set-up
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
}

void loop() {
  // MIC section
  int mic = digitalRead(TRIGGER);
  int current_millis = millis();

  if (!mic && claps <= 2) { // !mic is a clap // no more than 2 claps in a row
    claps += 1;
    if ((current_millis - first_clap_millis) >= 200 && (current_millis - first_clap_millis) <= 800) {
      //Serial.println("double mic!");
      
      switch_leds();

      claps = 0;
    } // if
    first_clap_millis = current_millis;
    claps = 0;
  } // if 

  if (leds[0].r > 0 && leds[0].g > 0 && leds[0].b > 0 ) {
    fill_solid(leds, NUM_LEDS, ColorFromPalette(myPal, GRADIENT_PALLETE_END));
    FastLED.show();
  }
  //   while (BT.available() > 0) {
  //     int redInt = BT.parseInt();
  //     int greenInt = BT.parseInt();
  //     int BTeInt = BT.parseInt();
  //     redInt = constrain(redInt, 0, 255);
  //     greenInt = constrain(greenInt, 0, 255);
  //     BTeInt = constrain(BTeInt, 0, 255);
  //     if (BT.available() > 0)
  //     {
  //       // Serial.print("Red: ");
  //       // Serial.print(redInt);
  //       // Serial.print(" Green: ");
  //       // Serial.print(greenInt);
  //       // Serial.print(" BTe: ");
  //       // Serial.print(BTeInt);
  //       // Serial.println();
  //       fill_solid(leds, NUM_LEDS, CRGB(redInt, greenInt, BTeInt));
  //       FastLED.show();
  //       BT.flush();
  //     }
  //   }
  // }
}
/*
  Toggle LEDs on and off. You can only
  set colours when 0 LED is on. 
*/
void switch_leds() {
  //Serial.println(leds[0].r);
  if (leds[0].r == 0 && leds[0].g == 0 && leds[0].b == 0 ) {
    for (uint16_t i = 0; i <= NUM_LEDS / 2; i++) {
      for (int index = 0; index <= GRADIENT_PALLETE_END; index += 20) {
        leds[i] = ColorFromPalette(myPal, index);
        leds[NUM_LEDS - i] = ColorFromPalette(myPal, index);

        FastLED.show();
        delay(10);
      }// for
    delay(timeDelay);
    }
  } else {
    //Serial.println("LOW");
    for (uint16_t i = 15; i <= NUM_LEDS; i++) {
      for (int index = GRADIENT_PALLETE_END; index >= 0; index -= 20) {
        leds[i] = ColorFromPalette(myPal, index);
        leds[NUM_LEDS - i] = ColorFromPalette(myPal, index);

        FastLED.show();
        delay(10);
      } // for
    delay(timeDelay);
    } // for
  }
  
}
