#include <Arduino.h>
#include <HCSR04.h>

#define TRIG_PIN 35
#define ECHO_PIN 37

UltraSonicDistanceSensor distanceSensor(TRIG_PIN, ECHO_PIN);

void setup() {
  Serial.begin(115200);
}

void loop() {
  int distance = distanceSensor.measureDistanceCm();  // Get distance in cm

  if (distance <= 0) {
    Serial.println("Error: No valid reading from sensor");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(500);
}
