#include <FastLED.h>

#define LED_PIN     10
#define NUM_LEDS    288
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define COOLING 75
#define SPARKING 120
#define BLOCK_SIZE  40
#define DECAY_STEP  10
#define FLASH_DELAY 100 
#define RIPPLE_SIZE 20
#define RIPPLE_SPEED 2 
CRGB leds[NUM_LEDS];
uint8_t rippleDirection = 1;
uint16_t xHue = 0;
uint16_t xBrightness = 0;
uint8_t gHue = 0;
CHSV runwayColor = CHSV(35, 255, 255);
const uint8_t BLOCK_SIZE_CONSTANT = 50;
const uint8_t GROUP_SIZE = 5;
int8_t fireDirection = 1;
int direction = 1;
const int blockLength = 60;
int whiteBlockStart = (NUM_LEDS - blockLength) / 2;
int consecutiveMoves = 0;
int hueShift = 0;
CHSV sb_currentColor;
int sb_center = NUM_LEDS / 2;
int sb_start = sb_center - BLOCK_SIZE / 2;
int sb_end = sb_center + BLOCK_SIZE / 2;
int8_t ripplePos = -1;
uint8_t rippleAge = 0;
uint32_t lastUpdateTime = 0;
int spread = 0;
CHSV currentColor;
int pulseBrightness = 0;
bool increasing = true;
const int PULSE_SPEED = 10;
const int SPREAD_SPEED = 5;
const uint16_t UPDATE_INTERVAL = 100;
const int TRAIL_LENGTH = 12;  // Adjust to change the length of the trails
const int NUM_SHOOTS = 3; 
struct Shoot {
    int posRed;
    int posBlue;
};
Shoot shoots[NUM_SHOOTS];
const int PLANET_SIZE = 5;

struct Planet {
    int position;
    bool active;
};

enum Mode {
    MODE_STARFIELD,
    MODE_FIRE,
    MODE_RUNWAY,
    MODE_COMPLEMENTARY_BLOCKS,
    MODE_RANDOM_COLORS_WHITE_BLOCK,
    MODE_RAINBOW_WAVES,
    SPREAD_BLOCKS,
    MODE_RIPPLE_WAVE,
    MODE_RAINBOW_CYCLE,
    MODE_PSYCHEDELIC_SWIRL,
    MODE_COLORFUL_STROBE,
    MODE_TRIPPY_WAVE,
    MODE_RISING_SUN,
    MODE_GALACTIC_WORM,
    MODE_MIRROR_RUN,
    MODE_RAIN_DROPS,
    MODE_HEARTBEAT,
    MODE_SHADOWS,
    MODE_VOID,
    MODE_ECLIPSE
};
Mode currentMode = MODE_STARFIELD;
unsigned long lastModeChange = 0;
const unsigned long MODE_DURATION = 1000;

void setup() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    setupMirrorRun();
}
void moveToNextMode() {
    switch (currentMode) {
        case MODE_STARFIELD:
            currentMode = MODE_FIRE;
            break;
        case MODE_FIRE:
            currentMode = MODE_RUNWAY;
            break;
        case MODE_RUNWAY:
            currentMode = MODE_COMPLEMENTARY_BLOCKS;
            break;
        case MODE_COMPLEMENTARY_BLOCKS:
            currentMode = MODE_RANDOM_COLORS_WHITE_BLOCK;
            break;
        case MODE_RANDOM_COLORS_WHITE_BLOCK:
            currentMode = MODE_RAINBOW_WAVES;
            break;
        case MODE_RAINBOW_WAVES:
            currentMode = SPREAD_BLOCKS;
            break;
        case SPREAD_BLOCKS:
            currentMode = MODE_RIPPLE_WAVE;
            break;
        case MODE_RIPPLE_WAVE:
            currentMode = MODE_RAINBOW_CYCLE;
            break;
        case MODE_RAINBOW_CYCLE:
            currentMode = MODE_PSYCHEDELIC_SWIRL;
            break;
        case MODE_PSYCHEDELIC_SWIRL:
            currentMode = MODE_COLORFUL_STROBE;
            break;
        case MODE_COLORFUL_STROBE:
            currentMode = MODE_TRIPPY_WAVE;
            break;
        case MODE_TRIPPY_WAVE:
            currentMode = MODE_RISING_SUN;
            break;
        case MODE_RISING_SUN:
            currentMode = MODE_GALACTIC_WORM;
            break;
        case MODE_GALACTIC_WORM:
            currentMode = MODE_MIRROR_RUN;
            break;
        case MODE_MIRROR_RUN:
            currentMode = MODE_RAIN_DROPS;
            break;
        case MODE_RAIN_DROPS:
            currentMode = MODE_HEARTBEAT;
            break;
        case MODE_HEARTBEAT:
            currentMode = MODE_SHADOWS;
            break;
        case MODE_SHADOWS:
            currentMode = MODE_VOID;
            break;
        case MODE_VOID:
            currentMode = MODE_ECLIPSE;
            break;
        case MODE_ECLIPSE:
            currentMode = MODE_STARFIELD;
            break;
    }
}

void loop() {
    unsigned long now = millis();

    if (now - lastModeChange > MODE_DURATION) {
        moveToNextMode();
        lastModeChange = now;
    }

    switch (currentMode) {
    case MODE_STARFIELD:
        starField();
        break;
    case MODE_FIRE:
        Fire2012WithDirection();
        break;
    case MODE_RUNWAY:
        runwayAnimation();
        delay(500);
        break;
    case MODE_COMPLEMENTARY_BLOCKS:
        complementaryBlocksAnimation();
        break;
    case MODE_RANDOM_COLORS_WHITE_BLOCK:
        randomColorsWithWhiteBlock();
        break;
    case MODE_RAINBOW_WAVES:
        rainbowWaves();
        break;
    case SPREAD_BLOCKS:
        spreadBlocks();
        break;
    case MODE_RIPPLE_WAVE:
        waveAnimation();
        break;
    case MODE_RAINBOW_CYCLE:
        rainbowCycle(gHue++);  // gHue will make it cycle through colors
        break;
    case MODE_PSYCHEDELIC_SWIRL:
        psychedelicSwirl(gHue++);
        break;
    case MODE_COLORFUL_STROBE:
        colorfulStrobe();
        break;
    case MODE_TRIPPY_WAVE:
        trippyWave();
        break;
    case MODE_RISING_SUN:
        risingSun();
        break;
    case MODE_GALACTIC_WORM:
        galacticWorm();
        break;
    case MODE_MIRROR_RUN:
        mirrorRun();
        break;
    case MODE_RAIN_DROPS:
        rainDrops();
        break;
    case MODE_HEARTBEAT:
        heartbeat();
        break;
    case MODE_SHADOWS:
        shadowsAndWhispers();
        break;
    case MODE_VOID:
        voidsPulse();
        break;
    case MODE_ECLIPSE: 
        eclipseOfHope();
        break;
    }
    FastLED.show();
    FastLED.delay(30);
}
// Insert all your functions here:
// 1. Starfield functions
void starField() {
    uint8_t hueBase = inoise8(xHue);
    uint8_t brightnessBase = inoise8(xBrightness);
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((hueBase + i * 10) % 60 + 160, 255, (brightnessBase + i * 15) % 100 + 55);
    }
    
    xHue += 2;
    xBrightness += 3;
  
    for(int i = 0; i < NUM_LEDS; i++) {
        if(random8() < 10) {
            leds[i] = CRGB::White;
        }
    }

    if(random8() < 30) {
        int pos = random16() % NUM_LEDS;
        int length = random8(5, 15);
        int end = min(pos + length, NUM_LEDS);
        for(int j = pos; j < end; j++) {
            leds[j] = CRGB::White;
        }
    }
}

// 2. Fire functions
void Fire2012WithDirection()
{
    static byte heat[NUM_LEDS];
    byte cooldown;

    // Step 1. Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++) {
        cooldown = random8(0, ((COOLING * 10) / NUM_LEDS) + 2);
        if (cooldown > heat[i]) {
            heat[i] = 0;
        } else {
            heat[i] = heat[i] - cooldown;
        }
    }

    // Step 2. Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3. Randomly ignite new sparks of heat near the bottom
    if (random8() < SPARKING) {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255)); // Hotter spark
    }

    // Step 4. Convert heat to LED colors
    for (int j = 0; j < NUM_LEDS; j++) {
        CRGB color = HeatColor(heat[j]);
        int pixelNumber;
        if (fireDirection == 1) {
    pixelNumber = (NUM_LEDS - 1) - j;
} else {
    pixelNumber = j;
}

        leds[pixelNumber] = color;
    }

    // Occasionally change direction
  if (random8() < 10) {  // 4% chance to change direction
    fireDirection *= -1;
}

}

// 3. Runway functions
void clearStrip() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void runwayAnimation() {
  int centerPos = 0;
  int runwayLength = 30;  // Half-length of the runway lights
  
  while (centerPos <= NUM_LEDS) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    for(int i = centerPos - runwayLength; i <= centerPos + runwayLength; i++) {
      if(i >= 0 && i < NUM_LEDS) {  // Check bounds
        int distanceFromCenter = abs(centerPos - i);
        int brightness = 255 - (255 * distanceFromCenter / runwayLength);
        
        leds[i] = CHSV(runwayColor.hue, runwayColor.sat, brightness);
      }
    }

    centerPos += 8;  // Adjust this value to control the speed.
    
    FastLED.show();
    delay(10);  // A short delay for visual continuity. Adjust or remove as desired.
  }
  clearStrip();  // Clear the strip after completing the run
}
// 4. Complementary blocks functions
void complementaryBlocksAnimation() {
    CHSV primaryColor = CHSV(random8(), 255, 255); // Random color
    CHSV complementaryColor = getComplementaryColor(primaryColor);

    int blockSize = NUM_LEDS / 16;  // Dividing the strip into 16 blocks

    for (int block = 0; block < 16; block++) {
        if (block % 2 == 0) { // If it's an even block
            fill_solid(&leds[block * blockSize], blockSize, primaryColor);
        } else { // If it's an odd block
            fill_solid(&leds[block * blockSize], blockSize, complementaryColor);
        }
    }

    FastLED.show();
    delay(250);  // Refresh rate of 0.25 seconds
}

CHSV getComplementaryColor(CHSV primary) {
    CHSV complementaryColor = primary;

    if (primary.hue <= 30 || primary.hue >= 330) { // Reds
        complementaryColor.hue = 180;
    } else if (primary.hue <= 90) { // Yellows
        complementaryColor.hue = 240;
    } else if (primary.hue <= 150) { // Greens
        complementaryColor.hue = 300;
    } else if (primary.hue <= 210) { // Cyans
        complementaryColor.hue = 0;
    } else if (primary.hue <= 270) { // Blues
        complementaryColor.hue = 60;
    } else { // Magentas
        complementaryColor.hue = 120;
    }

    return complementaryColor;
}
//Randomcolorswhiteblockscode
void randomColorsWithWhiteBlock() {
    // Random flashing
    for (int i = 0; i < NUM_LEDS; i++) {
        if (i < whiteBlockStart || i >= whiteBlockStart + blockLength) {
            leds[i] = CHSV(random8(), 255, random8(200, 255));
        }
    }

    // Move white block
    whiteBlockStart += direction;
    consecutiveMoves++;

    // Draw white block with wrap-around logic
    for (int i = 0; i < blockLength; i++) {
        int idx = (whiteBlockStart + i) % NUM_LEDS;
        if (idx < 0) idx += NUM_LEDS;  // Ensure the index is always positive
        leds[idx] = CRGB::White;
    }

    // Chance to change direction
    if (consecutiveMoves >= 50 && random8() < 5) {  // 5% chance after 50 consecutive moves
        direction = -direction;
        consecutiveMoves = 0;
    }

    FastLED.show();
    delay(25);  // Faster movement
}

void rainbowWaves() {
  for (int i = 0; i < NUM_LEDS; i++) {
    // We use the sin() function to get a wave pattern. We divide by a value to stretch out the wave length.
    // We then convert from the -1 to 1 range of sin() to 0 to 255 for hue.
    byte hue = int((sin(i + hueShift) + 1) * 127.5);
    leds[i] = CHSV(hue, 255, 255);
  }

  // Move the wave for the next frame
  hueShift++;

  FastLED.show();
  delay(50);  // Adjust for desired speed
}

void spreadBlocks() {
    int blockSize = BLOCK_SIZE_CONSTANT;
    int brightness = 255; // Assuming max brightness
    
    // Assuming you want to spread blocks with random colors. This is just an example.
    for (int i = 0; i < NUM_LEDS; i += blockSize) {
        CHSV color = CHSV(random8(), 255, brightness);
        for (int j = 0; j < blockSize && (i + j) < NUM_LEDS; j++) {
            leds[i + j] = color;
        }
    }
    FastLED.show();
    delay(FLASH_DELAY);
}

void waveAnimation() {
  static uint8_t waveStart = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness = (sin8((i + waveStart) * 4) / 2) + 128;
    uint8_t hue = 160 + random8(10);  // Slight hue variation for more dynamic water feel
    leds[i] = CHSV(hue, 255, brightness);
  }
  waveStart += 2;
}

void waveAndRippleAnimation() {
  static uint8_t waveStart = 0;
  
  // Update wave pattern
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness = (sin8((i + waveStart) * 4) / 2) + 128;
    uint8_t hue = 160 + random8(10);  // Slight hue variation for more dynamic water feel
    leds[i] = CHSV(hue, 255, brightness);
  }

  // Occasionally start a new ripple
  if(random8() < 10) {  // 4% chance to start a ripple
    startRipple();
  }

  // Move the ripple
  updateRipple();

  waveStart += 2;
  FastLED.show();  // Don't forget to update the display!
}
void startRipple() {
  ripplePos = random16() % NUM_LEDS;
  rippleAge = RIPPLE_SIZE / 2;
  rippleDirection = (random8() > 127) ? 1 : -1;  // Random starting direction
}

void updateRipple() {
  if (rippleAge <= 0) return;  // If ripple has faded completely, no need to continue
  
  // move the ripple
  ripplePos += RIPPLE_SPEED * rippleDirection;

  // Check for bounds and change direction if ripple hits either end of the strip
  if (ripplePos <= 0 || ripplePos >= NUM_LEDS - 1) {
    rippleDirection = -rippleDirection; // reverse the direction
  }
  
  for (int j = -RIPPLE_SIZE/2; j <= RIPPLE_SIZE/2; j++) {
    if (ripplePos + j >= 0 && ripplePos + j < NUM_LEDS) {
      uint8_t hue = 96; // Green hue for ripple
      uint8_t brightness = map(j, -RIPPLE_SIZE/2, RIPPLE_SIZE/2, 0, 255);
      brightness = max(brightness, 255 * rippleAge / (RIPPLE_SIZE / 2));
      leds[ripplePos + j] = CHSV(hue, 255, brightness); 
    }
  }
  
  rippleAge--;  // reduce ripple strength over time
}
void rainbowCycle(uint8_t initialHue) {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CHSV(initialHue + (i * 256L / NUM_LEDS), 255, 255);
    }
    FastLED.delay(20);
}

void psychedelicSwirl(uint8_t initialHue) {
    static uint8_t startHue = 0;
    startHue += 2; 
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CHSV(startHue + i * initialHue, 200, 255);
    }
    FastLED.delay(10);
}

void colorfulStrobe() {
    static uint8_t hue = 0;
    hue += 5;
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    FastLED.delay(100);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.delay(50);
}

void trippyWave() {
    static uint8_t wave1 = 0;
    static uint8_t wave2 = 0;
    wave1 += 3;
    wave2 += -4;
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CHSV(sin8(wave1 + i * 10), 255, 255) + CHSV(sin8(wave2 + i * 15), 255, 192);
    }
    FastLED.delay(20);
}
void risingSun() {
    static uint8_t sunriseHue = 0;  // starts from red (0)
    static uint8_t brightness = 0;

    fill_solid(leds, NUM_LEDS, CHSV(sunriseHue, 255, brightness));
    
    // Check if we've reached maximum brightness
    if (brightness == 255) {
        // If we're at yellow, reset to start
        if (sunriseHue >= 32 && sunriseHue < 64) { // 32 is orange, 64 is roughly yellow in HSV
            sunriseHue = 0;  // Reset to red hue
            brightness = 0;  // Reset brightness
        } else {
            sunriseHue += 1;  // Continue moving towards yellow hue
        }
    } else {
        brightness += 1;  // Increase brightness
        sunriseHue += 1;  // Gradually shift hue
    }
    
    FastLED.show();
    delay(50);
}


// For MODE_GALACTIC_WORM
void galacticWorm() {
    static int wormPosition = 0;
    static int wormLength = 20; // Starting length
    static Planet planet;      // Only one planet

    // If it's the start, initialize the planet
    if (wormLength == 20 && wormPosition == 0) {
        planet.active = true;
        planet.position = random(NUM_LEDS);
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // Check for planet consumption
    int wormEndPosition = (wormPosition + wormLength) % NUM_LEDS;
    bool consumed = false;
    if (wormPosition <= wormEndPosition) {
        if (planet.position >= wormPosition && planet.position < wormEndPosition) {
            consumed = true;
        }
    } else {
        if (planet.position >= wormPosition || planet.position < wormEndPosition) {
            consumed = true;
        }
    }

    if (consumed) {
        wormLength += 10;  // Grow the worm by 10
        planet.active = false;    // Deactivate the planet
    }

    // If the planet is inactive, regenerate it
    if (!planet.active) {
        planet.position = random(NUM_LEDS);
        planet.active = true;
    }

    // Now, draw the worm
    byte baseHue = random(192, 208);  // Purplish-blueish hue range
    byte hueIncrement = random(1, 5);
    for (int i = 0; i < wormLength; i++) {
        byte currentHue = (baseHue + i * hueIncrement) % 255;
        int currentPos = (wormPosition + i) % NUM_LEDS;   // Ensure wrapping
        if (currentPos < 0) currentPos += NUM_LEDS;      // Adjust for negative indices
        leds[currentPos] = CHSV(currentHue, 255, 192 + random(0, 64));
    }

    byte moveType = random(3);
    if (moveType == 0) {
        wormPosition += 1;
    } else if (moveType == 1) {
        wormPosition -= 1;
    } else {
        wormPosition += 2;  // Sometimes make a larger jump
    }

    // Ensure worm position wraps around correctly
    wormPosition = (wormPosition + NUM_LEDS) % NUM_LEDS;

    // Draw the planet
    if (planet.active) {
        fill_solid(&leds[planet.position], PLANET_SIZE, CRGB::Coral);  // Coral color for the planet
    }

    // Reset worm if its length exceeds or equals the LED strip length
    if (wormLength >= NUM_LEDS) {
        wormLength = 20; // Reset to starting length
        wormPosition = 0; // Reset position
    }

    FastLED.show();
    delay(20);
}





// For MODE_MIRROR_RUN
void setupMirrorRun() {
    for (int i = 0; i < NUM_SHOOTS; i++) {
        shoots[i].posRed = -(i * TRAIL_LENGTH);
        shoots[i].posBlue = NUM_LEDS - 1 + (i * TRAIL_LENGTH);
    }
}

void mirrorRun() {
    FastLED.clear();

    for (int i = 0; i < NUM_SHOOTS; i++) {
        for (int j = 0; j < TRAIL_LENGTH; j++) {
            int redPos = shoots[i].posRed + j;
            int bluePos = shoots[i].posBlue - j;

            if (redPos >= 0 && redPos < NUM_LEDS) {
                leds[redPos] = CRGB::Red;
            }

            if (bluePos >= 0 && bluePos < NUM_LEDS) {
                leds[bluePos] = CRGB::Blue;
            }
        }

        shoots[i].posRed++;
        shoots[i].posBlue--;

        // Reset shoot when they meet or cross in the middle or if out of bounds
        if (shoots[i].posRed >= NUM_LEDS / 2 || shoots[i].posBlue <= NUM_LEDS / 2 || 
            shoots[i].posRed >= NUM_LEDS || shoots[i].posBlue < 0) {
            shoots[i].posRed = -(i * TRAIL_LENGTH);
            shoots[i].posBlue = NUM_LEDS - 1 + (i * TRAIL_LENGTH);
        }
    }

    FastLED.show();
    delay(30);
}


// For MODE_RAIN_DROPS
void rainDrops() {
    int ledToLight = random(NUM_LEDS);
    leds[ledToLight] = CRGB::Blue;
    FastLED.show();
    fadeToBlackBy(leds, NUM_LEDS, 50);
    delay(10);
}

// For MODE_HEARTBEAT
void heartbeat() {
    static uint8_t step = 0;
    static const uint8_t maxSteps = 10;  // Determines the speed of dimming after the pulse
    uint8_t brightness;

    if (step == 0) {
        brightness = 255;  // Initial strong pulse
    } else {
        // Gradual dimming after the pulse
        brightness = 255 * (maxSteps - step) / maxSteps;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        if (i % 4 == 0) {  // For every 4th LED, make it blue
            leds[i] = CHSV(160, 255, brightness); // 160 is a hue value for blue
        } else {
            leds[i] = CHSV(0, 255, brightness); // 0 is a hue value for red
        }
    }

    FastLED.show();

    if (++step > maxSteps) {
        step = 0;
        delay(600); // Delay for the next heartbeat (gives about 100 beats per minute)
    } else {
        delay(30);  // Delay for dimming steps
    }
}
void shadowsAndWhispers() {
    static int counter = 0;

    for (int i = 0; i < NUM_LEDS; i++) {
        if (random(100) < 2) { // 2% chance for a brighter flicker
            leds[i] = CHSV(0, 0, random(50, 100));
        } else {
            leds[i] = CHSV(0, 0, random(5, 15)); // Mostly dim lights
        }
    }

    FastLED.show();
    delay(100);

    counter++;
    if (counter > 100) {
        FastLED.clear();
        counter = 0;
    }
}
void voidsPulse() {
    static uint8_t brightness = 50;
    static int8_t direction = 1;

    brightness += direction * 5;
    if (brightness <= 50 || brightness >= 250) {
        direction = -direction; // Reverse the direction when reaching max/min
    }

    fill_solid(leds, NUM_LEDS, CHSV(200, 255, brightness)); // Deep blue pulse
    FastLED.show();
    delay(50);
}

void eclipseOfHope() {
    static int sunEnd = 0;                 // End of the sun segment
    static int moonStart = NUM_LEDS;       // Start of the moon segment

    fill_solid(leds, NUM_LEDS, CRGB::Black);    // Initialize to darkness

    int segmentSize = NUM_LEDS / 2;            // Both sun and moon are half the strip size

    // Draw sun, if it's not fully out of the LED strip yet
    if (sunEnd <= NUM_LEDS) {
        int sunSize = min(sunEnd, segmentSize);  // Sun grows until it reaches segmentSize
        fill_solid(leds, sunSize, CHSV(30, 255, 255));     // Yellowish sun
    }

    // Draw moon, if it has appeared on the strip
    if (moonStart < NUM_LEDS) {
        int moonSize = min(NUM_LEDS - moonStart, segmentSize);  // Moon appears gradually until it reaches segmentSize
        fill_solid(&leds[moonStart], moonSize, CHSV(0, 0, 255));  // White moon
    }

    FastLED.show();
    delay(50);  // Adjust for desired speed

    // Move sun's end and moon's start
    sunEnd++;
    moonStart--;

    // Reset when both sun and moon are completely off the strip
    if (sunEnd >= segmentSize + NUM_LEDS && moonStart <= -segmentSize) {
        sunEnd = 0;
        moonStart = NUM_LEDS;
    }
}











