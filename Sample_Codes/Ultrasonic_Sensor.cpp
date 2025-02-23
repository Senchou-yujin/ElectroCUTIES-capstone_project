// Include NewPing Library
#include "NewPing.h"

#define TRIGGER_PIN 6
#define ECHO_PIN 7

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400	

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // Serial Monitor Setup
	Serial.begin(115200);
}

void loop() {
	Serial.print("Distance = ");
	int iterations = 5;
  Serial.print((sonar.ping_median(iterations) / 2) * 0.0343);
	Serial.println(" cm");
	delay(500);
}