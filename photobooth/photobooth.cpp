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
const int BUTTON_LED_PIN = 4;
const int BUTTON_PIN = 5;
const int NEOPIXEL_PIN = 6;


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


void standby() {
    debugPrint("Starting standby.");
    lightButton(true);

    int frequency = 10;
    int startTime = millis();
    int lastTime = startTime;
    SineEase ease;

    // The duration is half of the cycle (e.g., dark to light), so the full
    // cycle (e.g. dark to light to dark again) takes twice as long.
    ease.setDuration(2500); // half cycle in milliseconds

    // The total change in position controls the maximum brightness of the
    // NeoPixels. Because the NeoPixels have a range of 255 to 0, a total
    // change in position less than about 1/3rd the total brightness makes the
    // stops between values obvious as a sort of stutter in the animation.
    ease.setTotalChangeInPosition(255/3);

    double easedPosition;

    while (true) {
        if (millis() - lastTime > frequency) { // min change frequency is 10ms
            lastTime = millis();
            easedPosition = ease.easeInOut((double)(lastTime - startTime));
            setStrip(strip.Color(0, (int)easedPosition, 0));
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

    // LED states
    lightButton(true);
    lightSMD(true);

    // Neopixel
    strip.begin();
    strip.show();
    setStrip(WHITE);
}


void loop() {
    standby();
    delay(1000);
}
