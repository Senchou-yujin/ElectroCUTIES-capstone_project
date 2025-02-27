#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "NewPing.h"

//Pin Designations
#define buttPin1 22   // Pin for Button 1
#define buttPin2 24   // Pin for Button 2
#define buttPin3 26   // Pin for Button 3
#define buttPin4 28   // Pin for Button 4
#define BUZZER_PIN 30 // Pin for Buzzer Module
//Universal Coin Selector
#define COIN_PIN 2  // Coin acceptor signal pin
//Mosfet Module
#define MOSFET_Ariel 3  // Pump for Ariel
#define MOSFET_Downy 4  // Pump for Downy
#define MOSFET_Joy 5  // Pump for Joy
// Ariel Liquid Detergent Indicator LED
#define RED_ARIEL 32
#define YELLOW_ARIEL 34
#define GREEN_ARIEL 36
// Downy Fabric Softener Indicator LED
#define RED_DOWNY 38
#define YELLOW_DOWNY 40
#define GREEN_DOWNY 42
// Joy Dishwashing Liquid Indicator LED
#define RED_JOY 44
#define YELLOW_JOY 46
#define GREEN_JOY 48
//Ultrasonic Sensor for customer verification
//Ariel
#define TRIGGER_ARIEL 23
#define ECHO_ARIEL 25
//Downy
#define TRIGGER_DOWNY 27
#define ECHO_DOWNY 29
//Joy
#define TRIGGER_JOY 31
#define ECHO_JOY 33

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 15	

//Setup LCD Display
LiquidCrystal_I2C lcd(0x27, 20, 4); // Adjust address if needed
// NewPing setup of pins and maximum distance.
NewPing ariel(TRIGGER_ARIEL, ECHO_ARIEL, MAX_DISTANCE);
NewPing downy(TRIGGER_DOWNY, ECHO_DOWNY, MAX_DISTANCE);
NewPing joy(TRIGGER_JOY, ECHO_JOY, MAX_DISTANCE);

volatile int coinCount = 0;
volatile bool coinInserted = false;
volatile unsigned long lastInterruptTime = 0;
unsigned long lastScreenChange = 0;
bool showStartScreen = true;
bool selectionMade = false;
bool paymentScreenActive = false;
int selectedProduct = 0;
unsigned long selectionTime = 0;
unsigned long lastBlinkTime = 0;
bool textVisible = true;

void coinISR() {
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 100) {  // Adjust debounce if needed
    delay(5);  // Helps filter out noise
    coinInserted = true;
   //Serial.println("ISR Triggered");  // Debug message
  }
  
  lastInterruptTime = interruptTime;
}

void startScreen() {
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("Welcome!");
  lcd.setCursor(0, 2);
  lcd.print("Select Your Product.");
}


void arielPaymentConfirmation(int amount) {
  int arielVolume = (amount / 5) * 20 + (amount % 5) * 4;  // Ariel: PHP 5.00 per 20ml

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Amount: PHP ");
  lcd.print(amount);

  lcd.setCursor(0, 1);
  lcd.print("Ariel : ");
  lcd.print(arielVolume);
  lcd.print(" ml");

  lcd.setCursor(0, 3);
  lcd.print("##PRESS DONE##");
}

void downyPaymentConfirmation(int amount) {
  int downyVolume = (amount / 5) * 18 + (amount % 5) * 3.6;  // Downy: PHP 5.00 per 18ml

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Amount: ");
  lcd.print(amount);

  lcd.setCursor(0, 1);
  lcd.print("Downy : ");
  lcd.print(downyVolume);
  lcd.print(" ml");

  lcd.setCursor(0, 3);
  lcd.print("##PRESS DONE##");
}

void joyPaymentConfirmation(int amount) {
  int joyVolume = (amount / 5) * 16 + (amount % 5) * 3.2;  // Joy: PHP 5.00 per 16ml

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Amount: PHP ");
  lcd.print(amount);

  lcd.setCursor(0, 1);
  lcd.print("Joy : ");
  lcd.print(joyVolume);
  lcd.print(" ml");

  lcd.setCursor(0, 3);
  lcd.print("##PRESS DONE##");
}


void productSelection() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Your Product:");
  lcd.setCursor(2, 1);
  lcd.print("1. Ariel Detergent");
  lcd.setCursor(2, 2);
  lcd.print("2. Downy Softener");
  lcd.setCursor(2, 3);
  lcd.print("3. Joy Dishwashing");
}

void displayChosenProduct(String product) {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Selected Product:");
  lcd.setCursor(4, 1);
  lcd.print(">> ");
  lcd.print(product);
  lcd.setCursor(1, 3);
  lcd.print("Enter your Payment.");
}

void dispensingProduct(String product) {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Dispensing...");
  lcd.setCursor(6, 2);
  lcd.print(product);
}

void endScreen() {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Thank You!");
  delay(3000); // 3-second delay
}

void setup() {
  //Set up Serial for Debugging
  Serial.begin(115200);

  //Set up LCD
  lcd.init();
  lcd.backlight();
  startScreen();

  //Set Up Pin Modes
  pinMode(buttPin1, INPUT_PULLUP);
  pinMode(buttPin2, INPUT_PULLUP);
  pinMode(buttPin3, INPUT_PULLUP);
  pinMode(buttPin4, INPUT_PULLUP);
  //Universal Coin Selector
  pinMode(COIN_PIN, INPUT_PULLUP);  // Use internal pull-up or try external 10kΩ
  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinISR, RISING);
  //MOSFETS
  pinMode(MOSFET_Ariel, OUTPUT);
  pinMode(MOSFET_Downy, OUTPUT);
  pinMode(MOSFET_Joy, OUTPUT);
  //LEDs
  // Ariel Fabric Softener Indicator LED
  pinMode(RED_ARIEL, OUTPUT);
  pinMode(YELLOW_ARIEL, OUTPUT);
  pinMode(GREEN_ARIEL, OUTPUT);
  // Downy Fabric Softener Indicator LED
  pinMode(RED_DOWNY, OUTPUT);
  pinMode(YELLOW_DOWNY, OUTPUT);
  pinMode(GREEN_DOWNY, OUTPUT);
  // Joy Dishwashing Liquid Indicator LED
  pinMode(RED_JOY, OUTPUT);
  pinMode(YELLOW_JOY, OUTPUT);
  pinMode(GREEN_JOY, OUTPUT);

  // Ensure all components are OFF initially
  digitalWrite(RED_ARIEL, LOW);
  digitalWrite(YELLOW_ARIEL, LOW);
  digitalWrite(GREEN_ARIEL, LOW);
  digitalWrite(RED_DOWNY, LOW);
  digitalWrite(YELLOW_DOWNY, LOW);
  digitalWrite(GREEN_DOWNY, LOW);
  digitalWrite(RED_JOY, LOW);
  digitalWrite(YELLOW_JOY, LOW);
  digitalWrite(GREEN_JOY, LOW);
  digitalWrite(MOSFET_Ariel, LOW);
  digitalWrite(MOSFET_Downy, LOW); 
  digitalWrite(MOSFET_Joy, LOW); 
  
}

void loop() {
  unsigned long currentMillis = millis();
  int selectButton = 0;

  if (!selectionMade && (currentMillis - lastScreenChange >= 3000)) {
    lastScreenChange = currentMillis;

    if (showStartScreen) {
      startScreen();
    } else {
      productSelection();
    }
    
    showStartScreen = !showStartScreen;
  }

  if (digitalRead(buttPin1) == LOW) {
      Serial.println("Button 1 Pressed!");
      selectButton = 1;
  }
  if (digitalRead(buttPin2) == LOW) {
      Serial.println("Button 2 Pressed!");
      selectButton = 2;
  }
  if (digitalRead(buttPin3) == LOW) {
      Serial.println("Button 3 Pressed!");
      selectButton = 3;
  }
  if (digitalRead(buttPin4) == LOW) {
      Serial.println("Button 4 Pressed!");
      selectButton = 4;
  }


// Handle product selection
  if (selectButton > 0 && !selectionMade) {
    selectionMade = true;
    lcd.clear();
    
    switch (selectButton) {
      case 1:
        displayChosenProduct("Ariel");
        selectedProduct = 1;
        break;
      case 2:
        displayChosenProduct("Downy");
        selectedProduct = 2;
        break;
      case 3:
        displayChosenProduct("Joy");
        selectedProduct = 3;
        break;
    }
  }

    // Blinking "Enter your Payment." text
  if (selectionMade && !paymentScreenActive) {
    if (currentMillis - lastBlinkTime >= 500) { // Blink every 500ms
      lastBlinkTime = currentMillis;
      textVisible = !textVisible;

      if (textVisible) {
        lcd.setCursor(1, 3);
        lcd.print("Enter your Payment.");
      } else {
        lcd.setCursor(1, 3);
        lcd.print("                   "); // Clear the line
      }
    }
  }

  // Handle coin insertion
  if (coinInserted) {
    coinInserted = false;
    coinCount++;

    Serial.print("Coin inserted. Total: ");
    Serial.println(coinCount);

    // Switch to payment confirmation screen
    paymentScreenActive = true;
    switch (selectedProduct) {
      case 1:
        arielPaymentConfirmation(coinCount);
        break;
      case 2:
        downyPaymentConfirmation(coinCount);
        break;
      case 3:
        joyPaymentConfirmation(coinCount);
        break;
    }
  }
  //   // Reset after payment is completed (optional)
  // if (paymentScreenActive && coinCount >= requiredCoins) {
  //   // Reset for the next transaction
  //   selectionMade = false;
  //   paymentScreenActive = false;
  //   coinCount = 0;
  //   selectedProduct = "";
  // }
}




