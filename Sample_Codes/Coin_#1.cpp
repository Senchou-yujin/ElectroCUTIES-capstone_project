#include <Arduino.h>

#define COIN_PIN 2  // Coin acceptor signal pin

volatile int coinCount = 0;
volatile bool coinInserted = false;
volatile unsigned long lastInterruptTime = 0;

// Interrupt Service Routine (ISR)
void coinISR() {
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 100) {  // Adjust debounce if needed
    delay(5);  // Helps filter out noise
    coinInserted = true;
    Serial.println("ISR Triggered");  // Debug message
  }
  
  lastInterruptTime = interruptTime;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Coin counter started...");

  pinMode(COIN_PIN, INPUT_PULLUP);  // Use internal pull-up or try external 10kΩ
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinISR, RISING);
}

void loop() {
  if (coinInserted) {
    coinInserted = false;
    coinCount++;

    Serial.print("Coin inserted. Total: ");
    Serial.println(coinCount);
  }
}
