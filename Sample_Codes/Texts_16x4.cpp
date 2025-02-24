#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust address if needed

void welcomeScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print("Enter payment.");
}

void paymentConfirmation(int amount, int volume) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Amount: ");
  lcd.print(amount);
  lcd.setCursor(0, 1);
  lcd.print("Volume: ");
  lcd.print(volume);
}

void productSelection() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose Product:");
  lcd.setCursor(0, 1);
  lcd.print("1.Ariel 2.Downy 3.Joy");
}

void dispensingProduct(String product) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dispensing...");
  lcd.setCursor(0, 1);
  lcd.print(product);
}

void thankYouScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Thank you!");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  delay(3000); // 3-second delay
}

void setup() {
  lcd.init();
  lcd.backlight();
  welcomeScreen();
}

void loop() {
  // Example sequence
  delay(2000);
  paymentConfirmation(10, 500); // Example values
  delay(2000);
  productSelection();
  delay(2000);
  dispensingProduct("Ariel");
  delay(2000);
  thankYouScreen();
  delay(2000);
  welcomeScreen();
}

