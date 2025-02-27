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
unsigned long selectionTime = 0;

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


void paymentConfirmation(int amount) {
  int arielVolume = (coinCount / 5) * 20;  // Ariel: PHP 5.00 per 20ml
  int downyVolume = (coinCount / 5) * 20;  // Downy: PHP 5.00 per 20ml
  int joyVolume = (coinCount / 5) * 15;    // Joy: PHP 5.00 per 15ml

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Total Amount: PHP ");
  lcd.print(coinCount);

  lcd.setCursor(0, 1);
  lcd.print("Ariel : ");
  lcd.print(arielVolume);
  lcd.print(" ml");

  lcd.setCursor(0, 2);
  lcd.print("Downy : ");
  lcd.print(downyVolume);
  lcd.print(" ml");

  lcd.setCursor(0, 3);
  lcd.print("Joy : ");
  lcd.print(joyVolume);
  lcd.print(" ml");
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


  if (selectButton > 0) {
    selectionMade = true;
    lcd.clear();
    
    switch (selectButton) {
      case 1:
        dispensingProduct("Ariel");
        break;
      case 2:
        dispensingProduct("Downy");
        break;
      case 3:
        dispensingProduct("Joy");
        break;
    }
    
    delay(500);
  }
}



