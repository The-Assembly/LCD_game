# Build An Arduino LCD Game 
Workshop Overview

The objective of this workshop is to build a game on different types of displays for the Arduino using both regular wiring and I2C communication approaches.
The Arduino is a low cost, open-source microcontroller board using the ATMega328 with 32KB of Flash memory storage.
By the end of this workshop, we should be able to set-up a game with Artificial Intelligence to play against with different levels of difficulty.

Components Needed for LCD Approach:

      1. Arduino Uno
      2. 16x2 LCD Display
      3. Resistor – 220Ω
      4. Potentiometer – 10kΩ
      5. Tactile Switch/Button
      6. Jumper Wires

Components Needed for OLED Approach:

         1. Arduino Uno
         2. OLED Display  
         3. Three Resistors – 10kΩ 
         4. Potentiometer – 10kΩ
         5. Three Tactile Switch/Button
         6. Jumper Wires

Software Used for both approaches:

      •	Arduino software

Library Used for LCD Approach:
    
    •	LiquidCrystal Library for the LCD.
    
    •	Bounce2 Library for digital debouncing.

Library Used for OLED Approach:

    •	Bounce2.h Library for the digital debouncing.
    
    •	SPI.h Library for Serial Peripheral Interfacing
    
    •	Wire.h Library for I2C communication
    
    •	Adafruit_GFX.h Library for OLED Display
    
    •	Adafruit_SSD1306.h Library for OLED Display
 
