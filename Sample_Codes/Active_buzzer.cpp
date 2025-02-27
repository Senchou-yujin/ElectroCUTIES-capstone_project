#include <Arduino.h>

#define BUZZER_PIN 30  // This pin controls the transistor

void setup() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW); // Ensure it's off initially
}

void beep(int duration) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn ON buzzer
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);  // Turn OFF buzzer
}

void beeptwice(int duration1, int duration2) {
    beep(duration1);
    delay(200);
    beep(duration2);
}

void loop() {
    beep(1000);  // Single beep for 1 second
    delay(2000);

    beeptwice(500, 500);  // Two quick beeps
    delay(3000);
}
