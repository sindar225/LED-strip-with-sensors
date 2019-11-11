#include <Arduino.h>
#include <FastLED.h>

#define TRIGGER 15

#define DATA_PIN    16
#define NUM_LEDS    30
#define BRIGHTNESS  250
#define LED_TYPE    WS2812B

CRGB leds[NUM_LEDS];

int first_clap_millis = 0;
int claps = 0;
bool led_state = LOW;
bool previous_led_state = LOW;

void turn_led_on();
void turn_led_off();

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER, INPUT);

  // LED set-up
  FastLED.addLeds<LED_TYPE, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
}

void loop() {
  // MIC section
  int mic = digitalRead(TRIGGER);
  int current_millis = millis();

  if (!mic && claps <= 2) { // !mic is a clap // no more than 2 claps in a row
    claps += 1;
    if ((current_millis - first_clap_millis) >= 200 && (current_millis - first_clap_millis) <= 800) {
      Serial.println("double mic!");
      led_state = !led_state;
      
      if (led_state) {
        turn_led_on();
      } else {
        turn_led_off();
      } // if

      previous_led_state = !previous_led_state;
      claps = 0;
    } // if

    first_clap_millis = current_millis;

    claps = 0;
  } // if 
}

void turn_led_on() {
  if (previous_led_state) { return; } // do nothing if previous state was HIGH

  for (int i=0; i<NUM_LEDS; i++) { // iterate through LEDs
    for (int n=0; n <= 255; n+=15 ) { // change brightness of a single LED (increase)
      leds[i] = CHSV(256, 100, n);
      delay(1);
      FastLED.show();
    }
    delay(5); // delay between LEDs
  }
  FastLED.show();
}

void turn_led_off() {
  if (!previous_led_state) { return; } // do nothing if previous state was LOW

  for (int i=0; i<NUM_LEDS; i++) { // iterate through LEDs
    for (int n=255; n >= 0; n-=15 ) { // change brightness of a single LED (decrease)
      leds[i] = CHSV(256, 100, n);
      delay(1);
      FastLED.show();
    }
    delay(5); // delay between LEDs
  }
  FastLED.clear();
}