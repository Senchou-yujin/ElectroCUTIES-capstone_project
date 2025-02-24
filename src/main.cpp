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
//Mosfet Module
#define MOSFET_Ariel 2  // Pump for Ariel
#define MOSFET_Downy 3  // Pump for Downy
#define MOSFET_Joy 4  // Pump for Joy
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
#define MAX_DISTANCE 100	

//Setup LCD Display
LiquidCrystal_I2C lcd(0x27, 20, 4); // Adjust address if needed
// NewPing setup of pins and maximum distance.
NewPing ariel(TRIGGER_ARIEL, ECHO_ARIEL, MAX_DISTANCE);
NewPing downy(TRIGGER_DOWNY, ECHO_DOWNY, MAX_DISTANCE);
NewPing joy(TRIGGER_JOY, ECHO_JOY, MAX_DISTANCE);

void setupLEDindicator(int redPin, int yellowPin, int greenPin) {
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);

    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
}

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

void startScreen() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Welcome!");
  lcd.setCursor(0, 2);
  lcd.print("Enter your payment.");
}

void paymentConfirmation(int amount, int volume) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Amount Entered:");
  lcd.setCursor(5, 1);
  lcd.print(amount);
  
  lcd.setCursor(1, 2);
  lcd.print("Volume Equivalent:");
  lcd.setCursor(5, 3);
  lcd.print(volume);
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
  pinMode(BUZZER_PIN, OUTPUT);
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
  
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(MOSFET_Ariel, LOW);
  digitalWrite(MOSFET_Downy, LOW); 
  digitalWrite(MOSFET_Joy, LOW); 
  
}

void loop() {
  // Example sequence
  delay(2000);
  paymentConfirmation(20, 1000); // Example values
  delay(2000);
  productSelection();
  delay(2000);
  dispensingProduct("Downy");
  delay(2000);
  endScreen();
  delay(2000);
  startScreen();
}


