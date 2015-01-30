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
    digitalWrite(BUTTON_LED_PIN, HIGH);
    digitalWrite(13, HIGH);
    debugPrint("On");
    delay(1000);

    digitalWrite(BUTTON_LED_PIN, LOW);
    digitalWrite(13, LOW);
    debugPrint("Off");
    delay(1000);
}
