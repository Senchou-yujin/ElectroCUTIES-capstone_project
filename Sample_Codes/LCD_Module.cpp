#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the I2C address of the LCD (commonly 0x27 or 0x3F)
#define I2C_ADDR 0x27

// Initialize the LCD with 20 columns and 4 rows
LiquidCrystal_I2C lcd(I2C_ADDR, 20, 4);

void setup() {
    Wire.begin(); // Initialize I2C
    lcd.init();   // Initialize LCD
    lcd.backlight(); // Turn on the backlight
    
    // Display test messages on different lines
    lcd.setCursor(0, 0);
    lcd.print("20x4 LCD Test");
    lcd.setCursor(0, 1);
    lcd.print("Line 2: Hello");
    lcd.setCursor(0, 2);
    lcd.print("Line 3: ESP32");
    lcd.setCursor(0, 3);
    lcd.print("Line 4: Success!");
}

void loop() {
    // Optional: Blink backlight to indicate it's running
    delay(1000);
    lcd.noBacklight();
    delay(500);
    lcd.backlight();
}
