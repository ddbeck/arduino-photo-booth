// =========
// Debugging
// =========
#define DEBUG


// =====================
// Headers and Libraries
// =====================
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <EasingLibrary.h>
#include "debug.h"


// ========
// Settings
// ========

// --- Pins ---
// Trinket Pro pins off-limits: 2, 7
const int SMD_LED_PIN = 13;
const int BUTTON_LED_PIN = 3;
const int BUTTON_PIN = 5;
const int NEOPIXEL_PIN = 6;
const int AUTOFOCUS_PIN = 10;
const int SHUTTER_PIN = 11;


// ========
// NeoPixel
// ========
const int NEOPIXEL_LENGTH = 16;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_LENGTH,
                                            NEOPIXEL_PIN,
                                            NEO_GRB + NEO_KHZ800);

// --- Colors ---
const uint32_t GREEN = strip.Color(0, 18, 0);
const uint32_t YELLOW = strip.Color(18, 18, 0);
const uint32_t RED = strip.Color(18, 0, 0);
const uint32_t WHITE = strip.Color(18, 18, 18);
const uint32_t FLASH = strip.Color(255, 255, 255);
const uint32_t OFF = strip.Color(0, 0, 0);


// =======
// Support
// =======
void lightLED(bool on, int pin, String name){
    // Turn on or off an LED.
    if (on) {
        digitalWrite(pin, HIGH);
        debugPrint(name + " LED on.");
    }
    else {
        digitalWrite(pin, LOW);
        debugPrint(name + " LED off.");
    }
}


void lightButton(bool on) {
    lightLED(on, BUTTON_LED_PIN, "Arcade button");
}


void lightButton(int brightness) {
    analogWrite(BUTTON_LED_PIN, brightness);
    // debugPrint(String("Setting arcade button to ") + (brightness / 255) + "%");
}


void lightSMD(bool on) {
    lightLED(on, SMD_LED_PIN, "SMD");
}


void setStrip(uint32_t color) {
    for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}


bool isButtonPressed() {
    bool firstRead = digitalRead(BUTTON_PIN) == HIGH ? true : false;
    delay(5);
    bool secondRead = digitalRead(BUTTON_PIN) == HIGH ? true : false;

    if (firstRead && secondRead) {
        debugPrint("Button depressed.");
    }

    return firstRead && secondRead;
}


void autofocus(bool on) {
    digitalWrite(AUTOFOCUS_PIN, on ? HIGH : LOW);
}


void shutter(bool on) {
    digitalWrite(SHUTTER_PIN, on ? HIGH : LOW);
}


// ======================
// Lights, Camera, Action
// ======================
typedef struct CountdownState {
    int duration;
    int pixels;
    uint32_t color;
    String command;
} CountdownState;

CountdownState countdownStates[10] = {
    {750, 16, GREEN},
    {750, 14, GREEN},
    {750, 12, GREEN},
    {750, 10, YELLOW},
    {750, 8, YELLOW},
    {750, 6, YELLOW},
    {750, 4, RED, "AUTOFOCUS_ON"},
    {750, 2, RED, "AUTOFOCUS_OFF"},
    {10, 16, FLASH, "SHUTTER_ON"},
    {0, 16, OFF, "SHUTTER_OFF"},
};


void setCountdownState(CountdownState state) {
    int pixelCount = strip.numPixels();

    int offset = (pixelCount - state.pixels) / 2;
    int startIndex = 0 + offset;
    int endIndex = pixelCount - 1 - offset;

    debugPrint(String("Setting pixels [") + startIndex + String(",") +
               endIndex + String("] to ") + state.color);
    for (int pixelIndex = 0; pixelIndex < strip.numPixels(); pixelIndex++) {
        bool inRange = (pixelIndex >= startIndex) && (pixelIndex <= endIndex);

        if (inRange) {
            strip.setPixelColor(pixelIndex, state.color);
        }
        else {
            strip.setPixelColor(pixelIndex, OFF);
        }
    }
    strip.show();
    debugPrint("Finished setting pixels.");
}


// ==============
// Intervalometer
// ==============


void controlCamera(String command) {
    if (String("AUTOFOCUS_ON") == command) {
        autofocus(true);
    }
    else if (String("AUTOFOCUS_OFF") == command) {
        autofocus(false);
    }
    else if (String("SHUTTER_ON") == command) {
        shutter(true);
    }
    else if (String("SHUTTER_OFF") == command) {
        shutter(false);
    }
    else {
        debugPrint("ERROR: unrecognized camera command.");
    }
}


void countdown() {
    debugPrint("Starting countdown.");

    int countdownStep = 0;
    int stepStartTime = millis();
    int readTime = stepStartTime;

    while (countdownStep < sizeof(countdownStates) / sizeof(CountdownState)) {
        debugPrint(String("Animating index ") + countdownStep);

        setCountdownState(countdownStates[countdownStep]);
        stepStartTime = millis();

        controlCamera(countdownStates[countdownStep].command);

        while (true) {
            readTime = millis();
            if (readTime - stepStartTime >= countdownStates[countdownStep].duration) {
                debugPrint(String("Finished index ") + countdownStep +
                    String(" after ") + (readTime - stepStartTime) +
                    String("ms."));
                countdownStep++;
                break;
            }
        }
    }
    debugPrint("Exiting countdown.");
}


void standby() {
    debugPrint("Starting standby.");
    lightButton(true);

    const int frequency = 10;
    const int maxButtonBrightness = 255;
    const int maxStripBrightness = 255;

    int startTime = millis();
    int lastTime = startTime;
    SineEase ease;

    // The duration is half of the cycle (e.g., dark to light), so the full
    // cycle (e.g. dark to light to dark again) takes twice as long.
    ease.setDuration(2000); // half cycle in milliseconds

    // The total change in position controls the maximum brightness of the
    // NeoPixels. Because the NeoPixels have a range of 255 to 0, a total
    // change in position less than about 1/3rd the total brightness makes the
    // stops between values obvious as a sort of stutter in the animation.
    ease.setTotalChangeInPosition(1.0);

    double easedPosition;

    while (true) {
        if (millis() - lastTime > frequency) { // min change frequency is 10ms
            lastTime = millis();
            easedPosition = ease.easeInOut((double)(lastTime - startTime));

            setStrip(strip.Color(0,
                                 (int)(easedPosition * maxStripBrightness),
                                 0));
            lightButton((int)((1.0 - easedPosition) * maxButtonBrightness));
        }

        if (isButtonPressed()) {
            lightButton(false);
            setStrip(OFF);
            debugPrint("Exiting standby.");
            return;
        }
    }
}


// =======
// Arduino
// =======
void setup() {
    #ifdef DEBUG
        Serial.begin(9600);
        pinMode(SMD_LED_PIN, OUTPUT);
    #endif

    // Pin modes
    pinMode(BUTTON_LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    pinMode(AUTOFOCUS_PIN, OUTPUT);
    pinMode(SHUTTER_PIN, OUTPUT);

    // LED states
    lightButton(true);
    lightSMD(true);

    // Neopixel
    strip.begin();
    strip.show();
}


void loop() {
    standby();
    countdown();
    delay(1000);
}
