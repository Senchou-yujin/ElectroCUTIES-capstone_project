#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "NewPing.h"
#include <EEPROM.h>

//Pin Designations
#define buttPin1 22   // Pin for Button 1
#define buttPin2 24   // Pin for Button 2
#define buttPin3 26   // Pin for Button 3
#define buttPin4 28   // Pin for Button 4
#define BUZZER_PIN 53 // Pin for Buzzer Module
#define Relay_PIN 7 // Pin for the Coin Slot Auto Shutdown
//Universal Coin Selector
#define COIN_PIN 2  // Coin acceptor signal pin
//Mosfet Module
#define MOSFET_Ariel 5  // Pump for Ariel
#define MOSFET_Downy 3  // Pump for Downy
#define MOSFET_Joy 4  // Pump for Joy
// Ariel Liquid Detergent Indicator LED
#define RED_ARIEL 30
#define YELLOW_ARIEL 32
#define GREEN_ARIEL 34
// Downy Fabric Softener Indicator LED
#define RED_DOWNY 40
#define YELLOW_DOWNY 38
#define GREEN_DOWNY 36
// Joy Dishwashing Liquid Indicator LED
#define RED_JOY 42
#define YELLOW_JOY 44
#define GREEN_JOY 46
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
// Ultrasonic Sensor for Stock Verification
// Stock Sensor for Ariel
#define TRIGGER_STOCK_Ariel 35
#define ECHO_STOCK_Ariel 37
// Stock Sensor for Downy
#define TRIGGER_STOCK_Downy 39
#define ECHO_STOCK_Downy 41
// Stock Sensor for Joy
#define TRIGGER_STOCK_Joy 43
#define ECHO_STOCK_Joy 45

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
unsigned long button4PressStart = 0; // To track when Button 4 is pressed
bool button4Held = false;           // To track if Button 4 is being held

//Stock
int remainVolumeAriel;
int remainVolumeDowny;
int remainVolumeJoy;
int totalVolume = 0;
 // Stock Re-Calculation
 int deductedVolumeAriel = 0;
 int deductedVolumeDowny = 0;
 int deductedVolumeJoy = 0;
 int availableVolumeAriel = 0;
 int availableVolumeDowny = 0;
 int availableVolumeJoy = 0;

void coinISR() {
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 100) {  // Adjust debounce if needed
    delay(5);  // Helps filter out noise
    coinInserted = true;
   //Serial.println("ISR Triggered");  // Debug message
  }
  
  lastInterruptTime = interruptTime;
}

void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);  // Beep duration (adjust as needed)
  digitalWrite(BUZZER_PIN, LOW);
}

void beeptwice() {
  beep();  
  delay(200);  // Pause between beeps
  beep();
}

void CoinSlotON() {
    digitalWrite(Relay_PIN, HIGH); // Activate relay (disable coin slot)
}

void CoinSlotOFF() {
    digitalWrite(Relay_PIN, LOW); // Deactivate relay (enable coin slot)
}

void startScreen() {
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("Welcome!");
  lcd.setCursor(0, 2);
  lcd.print("Select Your Product.");
}

int arielPaymentConfirmation(int amount) {
  int arielVolume = (amount / 5) * 18 + (amount % 5) * 4;  // Ariel: PHP 5.00 per 20ml

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

  return arielVolume;  // Return the calculated volume
}

int downyPaymentConfirmation(int amount) {
  int downyVolume = (amount / 5) * 18 + (amount % 5) * 3.6;  // Downy: PHP 5.00 per 18ml

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Amount: PHP ");
  lcd.print(amount);

  lcd.setCursor(0, 1);
  lcd.print("Downy : ");
  lcd.print(downyVolume);
  lcd.print(" ml");

  lcd.setCursor(0, 3);
  lcd.print("##PRESS DONE##");

  return downyVolume;  // Return the calculated volume
}

int joyPaymentConfirmation(int amount) {
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

  return joyVolume;  // Return the calculated volume
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

void ArielLEDs(int level) {
  digitalWrite(RED_ARIEL, LOW);
  digitalWrite(YELLOW_ARIEL, LOW);
  digitalWrite(GREEN_ARIEL, LOW);

  switch (level) {
    case 0: // Low level (Red)
      digitalWrite(RED_ARIEL, HIGH);
      break;
    case 1: // Medium level (Yellow)
      digitalWrite(YELLOW_ARIEL, HIGH);
      break;
    case 2: // High level (Green)
      digitalWrite(GREEN_ARIEL, HIGH);
      break;
    default:
      break;
  }
}

void DownyLEDs(int level) {
  digitalWrite(RED_DOWNY, LOW);
  digitalWrite(YELLOW_DOWNY, LOW);
  digitalWrite(GREEN_DOWNY, LOW);

  switch (level) {
    case 0: // Low level (Red)
      digitalWrite(RED_DOWNY, HIGH);
      break;
    case 1: // Medium level (Yellow)
      digitalWrite(YELLOW_DOWNY, HIGH);
      break;
    case 2: // High level (Green)
      digitalWrite(GREEN_DOWNY, HIGH);
      break;
    default:
      break;
  }
}

void JoyLEDs(int level) {
  digitalWrite(RED_JOY, LOW);
  digitalWrite(YELLOW_JOY, LOW);
  digitalWrite(GREEN_JOY, LOW);

  switch (level) {
    case 0: // Low level (Red)
      digitalWrite(RED_JOY, HIGH);
      break;
    case 1: // Medium level (Yellow)
      digitalWrite(YELLOW_JOY, HIGH);
      break;
    case 2: // High level (Green)
      digitalWrite(GREEN_JOY, HIGH);
      break;
    default:
      break;
  }
}


bool isContainerDetected(int productID) {
  int distance = 0;

  // Read distance from the appropriate ultrasonic sensor
  switch (productID) {
    case 1: // Ariel
      distance = ariel.ping_cm();
      break;
    case 2: // Downy
      distance = downy.ping_cm();
      break;
    case 3: // Joy
      distance = joy.ping_cm();
      break;
    default:
      return false; // Invalid product
  }

  // Check if the distance is within the acceptable range (e.g., 5-15 cm)
  return (distance > 0 && distance <= MAX_DISTANCE);
}

void dispensingProduct(String product, int productID, unsigned long dispensingTime) {
  // Step 1: Prompt the user to place the container
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Place container...");
  while (!isContainerDetected(productID)) {
      delay(500); // Check every 500 ms
  }

  // Step 2: Display "Dispensing..." message with a loading bar
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Dispensing...");
  lcd.setCursor(0, 1);
  lcd.print("[                ]"); // Empty loading bar

  // Step 3: Activate the corresponding MOSFET to start dispensing
  switch (productID) {
      case 1: // Ariel
          digitalWrite(MOSFET_Ariel, HIGH);
          break;
      case 2: // Downy
          digitalWrite(MOSFET_Downy, HIGH);
          break;
      case 3: // Joy
          digitalWrite(MOSFET_Joy, HIGH);
          break;
  }

  // Step 4: Update the loading bar incrementally
  unsigned long startTime = millis();
  unsigned long endTime = startTime + (dispensingTime * 1000);
  int barLength = 16; // Length of the loading bar
  while (millis() < endTime) {
      unsigned long elapsedTime = millis() - startTime;
      int progress = (elapsedTime * barLength) / (dispensingTime * 1000);
      lcd.setCursor(1, 1);
      for (int i = 0; i < barLength; i++) {
          if (i < progress) {
              lcd.print("#"); // Fill the bar
          } else {
              lcd.print(" "); // Keep the rest empty
          }
      }
      delay(100); // Update every 100 ms
  }

  // Step 5: Deactivate the MOSFET
  switch (productID) {
      case 1: // Ariel
          digitalWrite(MOSFET_Ariel, LOW);
          break;
      case 2: // Downy
          digitalWrite(MOSFET_Downy, LOW);
          break;
      case 3: // Joy
          digitalWrite(MOSFET_Joy, LOW);
          break;
  }

  // Step 6: Display "Dispensing Done!" message
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Dispensing Done!");
  delay(2000); // Show message for 2 seconds
}

// void dispensingProduct(String product, int productID, unsigned long dispensingTime) {
//   // Step 1: Prompt the user to place the container
//   lcd.clear();
//   lcd.setCursor(0, 1);
//   lcd.print("Place container...");
//   while (!isContainerDetected(productID)) {
//       delay(500); // Check every 500 ms
//   }

//   // Step 2: Display "Dispensing..." message
//   lcd.clear();
//   lcd.setCursor(4, 0);
//   lcd.print("Dispensing...");
//   lcd.setCursor(6, 1);
//   lcd.print(product);

//   // Step 3: Activate the corresponding MOSFET to start dispensing
//   switch (productID) {
//       case 1: // Ariel
//           digitalWrite(MOSFET_Ariel, HIGH);
//           break;
//       case 2: // Downy
//           digitalWrite(MOSFET_Downy, HIGH);
//           break;
//       case 3: // Joy
//           digitalWrite(MOSFET_Joy, HIGH);
//           break;
//   }
//   // Step 4: Blocking delay using delay()
//   delay(dispensingTime*1000);
//   Serial.print("Dispensing Time: ");
//   Serial.println(dispensingTime); // Debug message

//   // Step 5: Deactivate the MOSFET
//   switch (productID) {
//       case 1: // Ariel
//           digitalWrite(MOSFET_Ariel, LOW);
//           break;
//       case 2: // Downy
//           digitalWrite(MOSFET_Downy, LOW);
//           break;
//       case 3: // Joy
//           digitalWrite(MOSFET_Joy, LOW);
//           break;
//   }

//   // Step 6: Display "Dispensing Done!" message
//   lcd.clear();
//   lcd.setCursor(4, 1);
//   lcd.print("Dispensing Done!");
//   delay(2000); // Show message for 2 seconds
// }

void endScreen() {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Thank You!");
  delay(3000); // 3-second delay

  Serial.println("Restarting Arduino...");
  asm volatile ("jmp 0"); // Restart Arduino
}

void setup() {
  //Set up Serial for Debugging
  Serial.begin(115200);
  // Read remaining volumes from EEPROM
  EEPROM.get(0, remainVolumeAriel);
  EEPROM.get(sizeof(int), remainVolumeDowny);
  EEPROM.get(2 * sizeof(int), remainVolumeJoy);

  // If EEPROM values are invalid (e.g., uninitialized), set default values
  if (remainVolumeAriel <= 0 || remainVolumeAriel > 2500) remainVolumeAriel = 2500;
  if (remainVolumeDowny <= 0 || remainVolumeDowny > 2500) remainVolumeDowny = 2500;
  if (remainVolumeJoy <= 0 || remainVolumeJoy > 2500) remainVolumeJoy = 2500;

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
  pinMode(Relay_PIN, OUTPUT);
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
  // All Hardware is off
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
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(Relay_PIN, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  int selectButton = 0;

  // Handle alternating screens (start screen and product selection)
  if (!selectionMade && (currentMillis - lastScreenChange >= 3000)) {
    lastScreenChange = currentMillis;

    if (showStartScreen) {
      startScreen();
    } else {
      productSelection();
    }
    
    showStartScreen = !showStartScreen;
  }

  // Check for button presses
  if (digitalRead(buttPin1) == LOW) {
      Serial.println("Button 1 Pressed!");
      selectButton = 1;
      beep();
  }
  if (digitalRead(buttPin2) == LOW) {
      Serial.println("Button 2 Pressed!");
      selectButton = 2;
      beep();
  }
  if (digitalRead(buttPin3) == LOW) {
      Serial.println("Button 3 Pressed!");
      selectButton = 3;
      beep();
  }
    if (digitalRead(buttPin4) == LOW) {
      if (button4PressStart == 0) {
          button4PressStart = currentMillis; 
      } else if (currentMillis - button4PressStart >= 2000 && !button4Held) {
          button4Held = true;
          Serial.println("Restarting Arduino...");
          asm volatile ("jmp 0");
      }
  } else {
      button4PressStart = 0; 
      button4Held = false; 
  }
  // OUT OF STOCK LEDS
  if (selectButton > 0 && !selectionMade) {
    selectionMade = true;
    lcd.clear();

    switch (selectButton) {
      case 1: // Ariel selected
        if (remainVolumeAriel <= 200) { // Low level (Out of Stock)
          ArielLEDs(0);
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Ariel Out of Stock!");
          lcd.setCursor(0, 2);
          lcd.print("Hold DONE to Restart");
          delay(1000);
          beep();
          delay(500);
          beep();
          delay(500);
          beep();
          delay(500);
          while (true) {
            // Wait for Button 4 (DONE) to be held for 3 seconds
            if (digitalRead(buttPin4) == LOW) {
              if (button4PressStart == 0) {
                button4PressStart = millis(); // Start timing
              } else if (millis() - button4PressStart >= 3000) {
                Serial.println("Restarting Arduino...");
                asm volatile ("jmp 0"); // Restart Arduino
              }
            } else {
              button4PressStart = 0; // Reset the timer if Button 4 is released
            }
          }
        }
        displayChosenProduct("Ariel");
        beep();
        delay(1000);
        if (remainVolumeAriel <= 500) { // Medium level (Yellow)
          ArielLEDs(1);
        } else { // High level (Green)
          ArielLEDs(2);
        }
        lcd.setCursor(1, 2);
        lcd.print("Stock: ");
        lcd.print(remainVolumeAriel);
        lcd.print(" mL");

        selectedProduct = 1;
        break;

      case 2: // Downy selected
        if (remainVolumeDowny <= 200) { // Low level (Out of Stock)
          DownyLEDs(0);
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Downy Out of Stock!");
          lcd.setCursor(0, 2);
          lcd.print("Hold DONE to Restart");
          delay(1000);
          beep();
          delay(500);
          beep();
          delay(500);
          beep();
          delay(500);
          while (true) {
            // Wait for Button 4 (DONE) to be held for 3 seconds
            if (digitalRead(buttPin4) == LOW) {
              if (button4PressStart == 0) {
                button4PressStart = millis(); // Start timing
              } else if (millis() - button4PressStart >= 3000) {
                Serial.println("Restarting Arduino...");
                asm volatile ("jmp 0"); // Restart Arduino
              }
            } else {
              button4PressStart = 0; // Reset the timer if Button 4 is released
            }
          }
        }
        displayChosenProduct("Downy");
        beep();
        delay(1000);
        if (remainVolumeDowny <= 500) { // Medium level (Yellow)
          DownyLEDs(1);
        } else { // High level (Green)
          DownyLEDs(2);
        }
        lcd.setCursor(1, 2);
        lcd.print("Stock: ");
        lcd.print(remainVolumeDowny);
        lcd.print(" mL");

        selectedProduct = 2;
        break;

      case 3: // Joy selected
        if (remainVolumeJoy <= 200) { // Low level (Out of Stock)
          JoyLEDs(2);
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Joy Out of Stock!");
          lcd.setCursor(0, 2);
          lcd.print("Hold DONE to Restart");
          delay(1000);
          beep();
          delay(500);
          beep();
          delay(500);
          beep();
          delay(500);
          while (true) {
            // Wait for Button 4 (DONE) to be held for 3 seconds
            if (digitalRead(buttPin4) == LOW) {
              if (button4PressStart == 0) {
                button4PressStart = millis(); // Start timing
              } else if (millis() - button4PressStart >= 3000) {
                Serial.println("Restarting Arduino...");
                asm volatile ("jmp 0"); // Restart Arduino
              }
            } else {
              button4PressStart = 0; // Reset the timer if Button 4 is released
            }
          }
        }
        displayChosenProduct("Joy");
        beep();
        delay(1000);
        if (remainVolumeJoy <= 500) { // Medium level (Yellow)
          JoyLEDs(1);
        } else { // High level (Green)
          JoyLEDs(2);
        }
        lcd.setCursor(1, 2);
        lcd.print("Stock: ");
        lcd.print(remainVolumeJoy);
        lcd.print(" mL");

        selectedProduct = 3;
        break;
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
        case 1: // Ariel
            totalVolume = arielPaymentConfirmation(coinCount);
            deductedVolumeAriel = arielPaymentConfirmation(coinCount);
            break;
        case 2: // Downy
            totalVolume = downyPaymentConfirmation(coinCount);
            deductedVolumeDowny = downyPaymentConfirmation(coinCount);
            break;
        case 3: // Joy
            totalVolume = joyPaymentConfirmation(coinCount);
            deductedVolumeJoy = joyPaymentConfirmation(coinCount);
            break;
    }

    Serial.print("Total Volume: ");
    Serial.println(totalVolume);  // Debug: Print the calculated volume
  }
    //      // Wait for Button 4/DONE to start dispensing
    if (paymentScreenActive && digitalRead(buttPin4) == LOW) {
      // Ensure the required amount is reached before dispensing
      int requiredCoins = 5; // Example: PHP 5.00 minimum for all products
      if (coinCount >= requiredCoins) {
          // Calculate dispensing time based on the total volume
          int dispensingTime = 0;
  
          switch (selectedProduct) {
            case 1: // Ariel
                dispensingTime = (deductedVolumeAriel * 90) / 100; // 100 mL in 85 seconds
                remainVolumeAriel -= deductedVolumeAriel; // Update remaining stock
                EEPROM.put(0, remainVolumeAriel);
                break;
            case 2: // Downy
                dispensingTime = (deductedVolumeDowny * 60) / 100; // 100 mL in 60 seconds
                remainVolumeDowny -= deductedVolumeDowny; // Update remaining stock
                EEPROM.put(sizeof(int), remainVolumeDowny); // Save to EEPROM
                break;
            case 3: // Joy
                dispensingTime = (deductedVolumeJoy * 96) / 100; // 100 mL in 95 seconds
                remainVolumeJoy -= deductedVolumeJoy; // Update remaining stock
                EEPROM.put(2 * sizeof(int), remainVolumeJoy); // Save to EEPROM
                break;
        }
        Serial.print("Dispensing Time: ");
        Serial.println(dispensingTime);
  
        // Dispense the product
        dispensingProduct(selectedProduct == 1 ? "Ariel" : selectedProduct == 2 ? "Downy" : "Joy", selectedProduct, dispensingTime);
  
        // Show end screen (which now includes the restart logic)
        endScreen();
  
        // Reset for the next transaction
        selectionMade = false;
        paymentScreenActive = false;
        coinCount = 0;
        selectedProduct = 0;
        totalVolume = 0; // Reset total volume
      } else {
          // Display a message if the required amount is not reached
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Insufficient Payment");
          lcd.setCursor(0, 1);
          lcd.print("Min: P 5.00 Required");
          delay(3000); // Show the message for 3 seconds
      }
  }
}