#include <Arduino.h>

#define BUZZER_PIN 30  // Adjust to your actual pin

// Alarm Sound Coding
void beep(int tone0) {
  // Beep the Buzzer
  tone(BUZZER_PIN, tone0);
  delay(100);
  noTone(BUZZER_PIN);
}

void beeptwice(int tone1, int tone2) {
  // Beep the Buzzer twice
  tone(BUZZER_PIN, tone1);
  delay(100);
  tone(BUZZER_PIN, tone2);
  delay(100);
  noTone(BUZZER_PIN);
}

void setup() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, HIGH); // Ensure it's off initially
}

void loop() {
    beep(3000);    // Single beep at 2000 Hz
    delay(1000);

    beeptwice(2000, 3500); // Two beeps at different tones
    delay(2000);
}
