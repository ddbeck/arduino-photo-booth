#include <Arduino.h>

// ========
// Settings
// ========

// --- Pins ---
// Trinket Pro pins off-limits: 2, 7
const int SMD_LED_PIN = 13;
const int BUTTON_LED_PIN = 4;


// =========
// Debugging
// =========
#define DEBUG
#include <debug.h>


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
}


void loop() {
    lightButton(true);
    lightSMD(true);
    delay(1000);

    lightButton(false);
    lightSMD(false);
    delay(1000);
}