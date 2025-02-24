#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  // Robojax code for LCD with I2C
	// initialize the LCD, 
	lcd.begin();
 
	// Turn on the blacklight and print a message.
	lcd.backlight();
}

void loop()
{
  bool blinking = true;
  //start of loop Robojax code for LCD with I2C
  lcd.clear();
  lcd.setCursor (1,0);
  lcd.print("Hello, Eugene");
  lcd.blink();
	blinking = true;
  lcd.setCursor (1,1); // go to start of 2nd line
  lcd.print("How's Ur Day?");
  //  lcd.print(millis() / 1000);
  delay(5000);
 //end of loopcode Robojax code for LCD with I2C
}