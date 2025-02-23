#include <Arduino.h>
#define MOSFET_PIN 9 	

void setup() {
  pinMode(MOSFET_PIN, OUTPUT);
}

void loop() {
  // Increase brightness/speed
  for (int duty = 0; duty <= 255; duty++) {
    analogWrite(MOSFET_PIN, duty);  // Set PWM value (0-255)
    delay(10);  // Adjust speed of increase
  }
  
  delay(500);  // Hold max power

  // Decrease brightness/speed
  for (int duty = 255; duty >= 0; duty--) {
    analogWrite(MOSFET_PIN, duty);
    delay(10);
  }

  delay(500);  // Hold off state
}
