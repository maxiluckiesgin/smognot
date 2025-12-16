// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <DHT.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <MQ135.h>
#include "pitches.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define I2C_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.66" OLED breakout board, Uno: A4=SDA, A5=SCL, 5V powered



#define DHTPIN 12     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define PIN_MQ135 A0

const int LEDPIN = D7;
const int BUZZZERPIN = D8;
float humidity = 0.0;
float temperature = 0.0;
float correctedPPM = 0.0;
unsigned long previousMillisAlert = 0;
unsigned long previousMillisMeasure = 0;

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(PIN_MQ135);

void doAlert() {
  long interval = 250; //milliseconds
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisAlert >= interval) {
    // Save the current time as the last time you blinked the LED
    previousMillisAlert = currentMillis;
    // If the LED is off turn it on and vice versa
    if (digitalRead(LEDPIN) == LOW) {
      digitalWrite(LEDPIN, LOW);
      digitalWrite(LEDPIN, HIGH);
      tone(BUZZZERPIN, NOTE_E6, 250);
    } else {
      digitalWrite(LEDPIN, LOW);
    }
  }
}

void measureSensor(){
  long interval = 2500; //milliseconds
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisMeasure >= interval) {
    previousMillisMeasure = currentMillis;
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
  }
}

void draw(void) {
  u8g2.setFont(u8g2_font_t0_12b_te );
  u8g2.setCursor(0, 11);
  u8g2.print("T: "); u8g2.print(temperature);
  u8g2.print(" C");	
  u8g2.setCursor(0, 22);
  u8g2.print("H: "); u8g2.print(humidity); u8g2.print(" %");
  u8g2.setCursor(0, 33);
  u8g2.print("AQI: ");
  if(correctedPPM >= 0 && correctedPPM < 51){
    u8g2.print("Good");
    digitalWrite(LEDPIN, LOW);
    noTone(BUZZZERPIN);
  } else if(correctedPPM >= 51 && correctedPPM < 150){
    u8g2.print("Bad");
    digitalWrite(LEDPIN, LOW);
    noTone(BUZZZERPIN);
  } else if(correctedPPM >= 151){
    u8g2.print("Poor");
    doAlert();
  }
  u8g2.setCursor(0, 44);
  u8g2.print(correctedPPM);
  u8g2.print("ppm");
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  u8g2.begin();
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  measureSensor();
  
  u8g2.firstPage();
  do {
    draw();
  } while ( u8g2.nextPage() );
}
