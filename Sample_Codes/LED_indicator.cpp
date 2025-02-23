#include <Arduino.h>

#define RED_PIN 39
#define YELLOW_PIN 43
#define GREEN_PIN 41

void setup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
}

void loop() {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    delay(1000);

    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, HIGH);
    delay(1000);

    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    delay(1000);

    digitalWrite(GREEN_PIN, LOW);
}
