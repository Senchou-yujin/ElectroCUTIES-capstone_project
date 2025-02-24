#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // Adjust address if needed

void welcomeScreen() {
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

void thankYouScreen() {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Thank You!");
  lcd.setCursor(3, 2);
  lcd.print("Please wait 3 sec...");
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
  paymentConfirmation(20, 1000); // Example values
  delay(2000);
  productSelection();
  delay(2000);
  dispensingProduct("Downy");
  delay(2000);
  thankYouScreen();
  delay(2000);
  welcomeScreen();
}
