#include <Arduino.h>
#define MOSFET_PIN 9  // Connect to the MOSFET module's PWM input

void setup() {
  pinMode(MOSFET_PIN, OUTPUT);
  Serial.begin(115200);  // Start serial communication for debugging
}

void loop() {
  // Full PWM (MOSFET ON) for 1 minute
  Serial.println("MOSFET ON: Full PWM for 1 minute");
  analogWrite(MOSFET_PIN, 255);  
  delay(55000);  // 1 minute (60,000 ms)

  // Turn OFF (MOSFET OFF) for 10 minutes
  Serial.println("MOSFET OFF: Waiting for 10 minutes");
  analogWrite(MOSFET_PIN, 0);  
  delay(600000);  // 10 minutes (600,000 ms)
}
