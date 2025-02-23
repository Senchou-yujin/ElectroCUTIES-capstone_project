#include <Arduino.h>

#define SWITCH_PIN 7   // Change to your actual switch pin
#define LED_PIN 2     // Built-in LED on Mega

void setup() {
    pinMode(SWITCH_PIN, INPUT_PULLUP); // Use internal pull-up resistor
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); // Ensure LED is off initially
}

void loop() {
    if (digitalRead(SWITCH_PIN) == LOW) {  // Switch is pressed (LOW)
        digitalWrite(LED_PIN, HIGH);  // Turn ON LED
        delay(100);  // Debounce delay
    } else {
        digitalWrite(LED_PIN, LOW);  // Turn OFF LED
    }
}
