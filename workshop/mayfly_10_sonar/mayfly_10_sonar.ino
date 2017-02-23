// This Mayfly sketch parses data from MaxSonar serial data and prints it on the serial monitor and a SSD1306 OLED display

//Connect the MaxBotix sensor to the Mayfly's D4-5 Grove connector
//Connect the OLED display to the Mayfly's I2C Grove connector

// Import required libraries
#include <Arduino.h>
#include <SDL_Arduino_SSD1306.h>    // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <AMAdafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)
#include <SPI.h>            // For SPI comm (needed for not getting compile error)
#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <SoftwareSerial.h>

// Pin definitions
SDL_Arduino_SSD1306 display(4); // FOR I2C

int range;

SoftwareSerial sonarSerial(5, -1);            //define serial port for recieving data.

boolean stringComplete = false;

void setup()
{
  Serial.begin(57600);                                      //start serial port for display
  sonarSerial.begin(9600);                                 //start serial port for maxSonar
  pinMode(5, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Mayfly");
  display.println("Sonar demo");
  display.display();

  Serial.println("Mayfly MaxBotix sonar sensor rangefinder example");
  delay(3000);
}

void loop()
{
  range = EZreadSonar();
  if(stringComplete)
  {
    stringComplete = false;                                //reset sringComplete ready for next reading

    Serial.print("Range: ");
    Serial.print(range);
    Serial.println(" mm");

    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Range:");
    display.print(range);
    display.println(" mm");
    display.display();

    delay(700);
  }
}


int EZreadSonar()
{
  int result;
  char inData[5];                                          //char array to read data into
  int index = 0;

  sonarSerial.flush();                                     // Clear cache ready for next reading

  while (stringComplete == false) {
    //Serial.print("reading ");    //debug line

      if (sonarSerial.available())
    {
      char rByte = sonarSerial.read();                     //read serial input for "R" to mark start of data
      if(rByte == 'R')
      {
        //Serial.println("rByte set");
        while (index < 4)                                  //read next three character for range from sensor
        {
          if (sonarSerial.available())
          {
            inData[index] = sonarSerial.read();
            //Serial.println(inData[index]);               //Debug line

            index++;                                       // Increment where to write next
          }
        }
        inData[index] = 0x00;                              //add a padding byte at end for atoi() function
      }

      rByte = 0;                                           //reset the rByte ready for next reading

      index = 0;                                           // Reset index ready for next reading
      stringComplete = true;                               // Set completion of read to true
      result = atoi(inData);                               // Changes string data into an integer for use
    }
  }

  return result;
}
