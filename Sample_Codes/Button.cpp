#include <Arduino.h>

// Pin Designations
#define buttPin1 22   // Pin for Button 1
#define buttPin2 24   // Pin for Button 2
#define buttPin3 26   // Pin for Button 3
#define buttPin4 28   // Pin for Button 4

void setup() {
    Serial.begin(115200);
    
    pinMode(buttPin1, INPUT_PULLUP);
    pinMode(buttPin2, INPUT_PULLUP);
    pinMode(buttPin3, INPUT_PULLUP);
    pinMode(buttPin4, INPUT_PULLUP);
    
    Serial.println("Button test started...");
}

void loop() {
    if (digitalRead(buttPin1) == LOW) {
        Serial.println("Button 1 Pressed!");
        delay(200);  // Simple debounce
    }
    if (digitalRead(buttPin2) == LOW) {
        Serial.println("Button 2 Pressed!");
        delay(200);
    }
    if (digitalRead(buttPin3) == LOW) {
        Serial.println("Button 3 Pressed!");
        delay(200);
    }
    if (digitalRead(buttPin4) == LOW) {
        Serial.println("Button 4 Pressed!");
        delay(200);
    }
}
