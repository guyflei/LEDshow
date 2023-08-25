#include <FastLED.h>

#define NUM_LEDS 288
#define DATA_PIN 10

CRGB leds[NUM_LEDS];

// define an array of colors
CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Purple, CRGB::Orange};

// define an array to store the color of each LED at the stack height
CRGB stackColors[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
  int pos = 0;
  int dir = 1;
  int bounceHeight = NUM_LEDS;
  int stackHeight = 0;
  
  // set the initial ball size
  float ballSize = 72.0;
  
  // generate a random color index
  int colorIndex = random8(0, sizeof(colors) / sizeof(colors[0]));
  
  while (stackHeight < NUM_LEDS) {
    // clear the strip below the stack height
    for (int i = 0; i < NUM_LEDS - stackHeight; i++) {
      leds[i] = CRGB::Black;
    }
    
    // set the current ballSize LEDs to the current color
    for (int i = pos; i < pos + (int)ballSize && i < NUM_LEDS - stackHeight; i++) {
      leds[i] = colors[colorIndex];
    }
    
    // set the LEDs at the stack height to their respective colors
    for (int i = NUM_LEDS - stackHeight; i < NUM_LEDS; i++) {
      leds[i] = stackColors[i];
    }
    
    // update the position
    pos += dir;
    
    // check for bounce
    if (pos >= NUM_LEDS - stackHeight - (int)ballSize) {
      dir = -1;
      bounceHeight /= 2;
      if (bounceHeight <= (int)ballSize) {
        stackHeight += (int)ballSize;
        bounceHeight = NUM_LEDS - stackHeight;
        pos = 0;
        dir = 1;
        
        // store the current color in the stackColors array
        for (int i = NUM_LEDS - stackHeight; i < NUM_LEDS - stackHeight + (int)ballSize; i++) {
          stackColors[i] = colors[colorIndex];
        }
        
        // generate a new random color index
        colorIndex = random8(0, sizeof(colors) / sizeof(colors[0]));
        
        // decrease the ball size by multiplying it by .75, but not less than 1
        ballSize = max(ballSize * .75, 1.0);
      }
    } else if (pos == NUM_LEDS - bounceHeight - stackHeight - (int)ballSize) {
      dir = 1;
    }
    
    // update the strip
    FastLED.show();
    
    // calculate the current delay time based on the stack height
    int delayTime = map(stackHeight, 0, NUM_LEDS, 30, 1);
    
    // wait for a bit
    delay(delayTime);
        if (stackHeight == NUM_LEDS) {
      unsigned long startTime = millis();
      while (millis() - startTime < 5000) {
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CHSV(random8(),255,255);
        }
        FastLED.show();
        delay(100);
      }
      break;
    }
  }
}


