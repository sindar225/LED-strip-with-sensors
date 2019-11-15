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

//SoftwareSerial BT(8, 7);

int first_clap_millis = 0;
int claps = 0;
bool state = LOW;

/*
  LED Strip effects
*/
void switch_leds();
void switch_modes();
void starter_mode();
void rainbow_beat();
void beatwave();
void fill_grad();
void dot_beat();
uint8_t modes[] = { 0, 1, 2, 3, 4 };
uint8_t currentEffect;

uint8_t timeDelay = 5;  // time in ms.  Lower number is faster travel.

void setup() {
  //Serial.begin(9600);
  //BT.begin(9600);
  pinMode(TRIGGER, INPUT);

  // LED set-up
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
}

void loop() {
  // MIC section
  bool mic = !digitalRead(TRIGGER);
  int current_millis = millis();
  
  if (mic) { 
    if (state) {
      claps = 1;
    }
    
    if ((current_millis - first_clap_millis) >= 200 && (current_millis - first_clap_millis) <= 600) {
      claps = 2;
    } 
    first_clap_millis = current_millis;
  } 
  
  switch(claps) {
    case 1: switch_modes();
      break;
    case 2: switch_leds();
      break;
  } 

  Serial.println(state);
  if (state) {
    switch (currentEffect) {
      case 0: starter_mode();
        break;
      case 1: rainbow_beat();
        break;
      case 2: beatwave();
        break;
      case 3: fill_grad();
        break;
      case 4: dot_beat();
        break;
    }
    
  }
  FastLED.show();
  claps = 0;

  // if (leds[0].r > 0 && leds[0].g > 0 && leds[0].b > 0 ) {
  //   fill_solid(leds, NUM_LEDS, ColorFromPalette(myPal, GRADIENT_PALLETE_END));
  //   FastLED.show();
  // }
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
  state = !state;
  if (state) {
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
    //fadeTowardColor( leds, NUM_LEDS, ColorFromPalette(myPal, GRADIENT_PALLETE_END), 255);
    currentEffect = 0;
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

void switch_modes() {
  int modesLength = (sizeof(modes) / sizeof(modes[0])) - 1;
  modes[currentEffect] == modesLength ? currentEffect = modes[0] : currentEffect = modes[currentEffect + 1];
  //Serial.print("Array length: "); Serial.println(modesLength);
  //Serial.print("Mode: "); Serial.println(currentEffect);
}

void starter_mode() {
  fill_solid(leds, NUM_LEDS, ColorFromPalette(myPal, GRADIENT_PALLETE_END));
}

void rainbow_beat() {
  uint8_t beatA = beatsin8(17, 0, 255);                        // Starting hue
  uint8_t beatB = beatsin8(13, 0, 255);
  fill_rainbow(leds, NUM_LEDS, (beatA+beatB)/2, 8);            // Use FastLED's fill_rainbow routine.

} // rainbow_beat()

// Palette definitions
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
  
/*
    Beatwave parameters
*/
TBlendType currentBlending = LINEARBLEND;

void beatwave() {
  
  uint8_t wave1 = beatsin8(9, 0, 255);                        // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255, currentBlending); 
  }

  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }

} // beatwave()

void fill_grad() {
  
  uint8_t starthue = beatsin8(5, 0, 255);
  uint8_t endhue = beatsin8(7, 0, 255);
  
  if (starthue < endhue) {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);    // If we don't have this, the colour fill will flip around. 
  } else {
    fill_gradient(leds, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), BACKWARD_HUES);
  }
  
} // fill_grad()

/*
  Dot beat
*/

// Define variables used by the sequences.
int   thisdelay =   10;                                       // A delay value for the sequence(s)
uint8_t   count =   0;                                        // Count up to 255 and then reverts to 0
uint8_t fadeval = 224;                                        // Trail behind the LED's. Lower => faster fade.

uint8_t bpm = 30;

void dot_beat() {

  uint8_t inner = beatsin8(bpm, NUM_LEDS/4, NUM_LEDS/4*3);    // Move 1/4 to 3/4
  uint8_t outer = beatsin8(bpm, 0, NUM_LEDS-1);               // Move entire length
  uint8_t middle = beatsin8(bpm, NUM_LEDS/3, NUM_LEDS/3*2);   // Move 1/3 to 2/3

  leds[middle] = CRGB::Purple;
  leds[inner] = CRGB::Blue;
  leds[outer] = CRGB::Aqua;

  nscale8(leds,NUM_LEDS,fadeval);                             // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);

} // dot_beat()

