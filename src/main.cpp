#include <Arduino.h>
#include <FastLED.h>

#define TRIGGER 15

#define DATA_PIN    16
#define NUM_LEDS    30
#define BRIGHTNESS  255
#define GRADIENT_PALLETE_END 200
#define LED_TYPE    WS2812B
#define timeDelay 5  // time in ms for switch_leds().  Lower number is faster travel.

CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE( royal_BTe_gd ) {
  0,     0,  0,  0,   //black
255,   0,  0,  80}; // CRGB::RoyalBTe

CRGBPalette16 myPal = royal_BTe_gd;

//SoftwareSerial BT(8, 7);

int first_clap_millis = 0;
uint8_t claps = 0;
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
void inoise8_mover();
void inoise8_mover_routine();
void meteorShower();
void pulse();
uint8_t modes[] = { 0, 1, 2, 3, 4, 5, 6 };
uint8_t currentEffect;

void setup() {
  //Serial.begin(9600);
  //BT.begin(9600);
  pinMode(TRIGGER, INPUT);

  // LED set-up
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); 
}

void loop() {
  // MIC section
  bool mic = !digitalRead(TRIGGER);
  int current_millis = millis();
  
  if (mic) { 
    if (state) {
      claps = 1;
    }
    
    if ((current_millis - first_clap_millis) >= 200 && (current_millis - first_clap_millis) <= 1000) {
      Serial.println("Double clap!");
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

  //Serial.println(state);
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
      case 4: inoise8_mover();
        break;
      case 5: meteorShower();
        break;
      case 6: pulse();
        break;
    }
  //Serial.print("Current effect: "); Serial.println(currentEffect);
    
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
  uint8_t modesLength = (sizeof(modes) / sizeof(modes[0])) - 1;
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

/*
    Beatwave parameters
*/
// Palette definitions
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
  
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
  Mover
*/
 
CRGBPalette16 startPalette=LavaColors_p;
CRGBPalette16 endPalette=OceanColors_p;
 
uint16_t xscale = 30;                                         // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint16_t yscale = 30;                                         // Wouldn't recommend changing this on the fly, or the animation will be really blocky.

uint8_t maxChanges = 24;  

static int16_t dist = random16(12345);  

void inoise8_mover() {
  
    EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(startPalette, endPalette, maxChanges);   // AWESOME palette blending capability.
    
    inoise8_mover_routine();                                          // Update the LED array with noise at the new location
    fadeToBlackBy(leds, NUM_LEDS, 4);     
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    endPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }
 
  FastLED.show();
  
} // loop()

void inoise8_mover_routine() {

  uint8_t locn = inoise8(xscale, dist+yscale) % 255;          // Get a new pixel location from moving noise.
  uint8_t pixlen = map(locn,0,255,0,NUM_LEDS);                // Map that to the length of the strand.
  leds[pixlen] = ColorFromPalette(startPalette, pixlen, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.

  dist += beatsin8(10,1,8);                                                // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.                                             

} // inoise8_mover()

/*
  Meteor shower
*/

uint8_t hue = 32;
byte idex = 255;
byte meteorLength = 29;

void meteorShower(){
  // slide all the pixels down one in the array
  EVERY_N_MILLISECONDS(random8(40, 100)) {    
    memmove8( &leds[1], &leds[0], (NUM_LEDS - 1) * 3 );
  
    // increment the meteor display frame
    idex++;
    // make sure we don't drift into space
    if ( idex > meteorLength ) {
      idex = 0;
      // cycle through hues in each successive meteor tail
      hue += 32;  
    }
  
    // this switch controls the actual meteor animation, i.e., what gets placed in the
    // first position and then subsequently gets moved down the strip by the memmove above
    switch ( idex ) {
    case 0:
      leds[0] = CRGB(200,200,200);
      break;
    case 1:
      leds[0] = CHSV((hue - 20), 255, 210);
      break;
    case 2:
      leds[0] = CHSV((hue - 22), 255, 180);
      break;
    case 3:
      leds[0] = CHSV((hue - 23), 255, 150);
      break;
    case 4:
      leds[0] = CHSV((hue - 24), 255, 110);
      break;
    case 5:
      leds[0] = CHSV((hue - 25), 255, 90);
      break;
    case 6:
      leds[0] = CHSV((hue - 26), 160, 60);
      break;
    case 7:
      leds[0] = CHSV((hue - 27), 140, 40);
      break;
    case 8:
      leds[0] = CHSV((hue - 28), 120, 20);
      break;
    case 9:
      leds[0] = CHSV((hue - 29), 100, 20);
      break;
    default:
      leds[0] = CRGB::Black;
    }
  }
}

/*
  Pulse
*/

static float pulseSpeed = 0.8;  // Larger value gives faster pulse.

uint8_t hueA = 130;  // Start hue at valueMin.
uint8_t satA = 200;  // Start saturation at valueMin.
float valueMin = 120.0;  // Pulse minimum value (Should be less then valueMax).

uint8_t hueB = 190;  // End hue at valueMax.
uint8_t satB = 255;  // End saturation at valueMax.
float valueMax = 200.0;  // Pulse maximum value (Should be larger then valueMin).

uint8_t pulse_hue = hueA;  // Do Not Edit
uint8_t sat = satA;  // Do Not Edit
float val = valueMin;  // Do Not Edit
uint8_t hueDelta = hueA - hueB;  // Do Not Edit
static float delta = (valueMax - valueMin) / 2.35040238;  // Do Not Edit

void pulse() {
  EVERY_N_MILLISECONDS(20) {
    float dV = ((exp(sin(pulseSpeed * millis()/2000.0*PI)) -0.36787944) * delta);
    val = valueMin + dV;
    pulse_hue = map(val, valueMin, valueMax, hueA, hueB);  // Map hue based on current val
    sat = map(val, valueMin, valueMax, satA, satB);  // Map sat based on current val

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(pulse_hue, sat, val);
    }
  }

}