//
//#include <Wire.h>
//#include "MAX30100_PulseOximeter.h"
//
//#define REPORTING_PERIOD_MS     1000
//
//PulseOximeter pox;
//uint32_t tsLastReport = 0;
//
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#include <Fonts/FreeSerif9pt7b.h>
//
//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//
//// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//
//
//
//void onBeatDetected()
//{
//  Serial.println("Beat!");
//}
//
//void setup()
//{
//  Serial.begin(115200);
//  Serial.print("Initializing pulse oximeter..");
//
//  if (!pox.begin()) {
//    Serial.println("FAILED");
//    for (;;);
//  } else {
//    Serial.println("SUCCESS");
//  }
//  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
//
//  // Register a callback for the beat detection
//  pox.setOnBeatDetectedCallback(onBeatDetected);
//
//
//  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//    Serial.println("SSD1306 allocation failed");
//    for (;;);
//  }
//  delay(2000);
//
//  display.setFont(&FreeSerif9pt7b);
//  display.clearDisplay();
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 20);
//  display.println("Hello, world!");
//  display.display();
//  delay(2000);
//}
//
//void loop()
//{
//  pox.update();
//  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
//    Serial.print("Heart rate:");
//    Serial.print(pox.getHeartRate());
//    Serial.print("bpm / SpO2:");
//    Serial.print(pox.getSpO2());
//    Serial.println("%");
//
//    tsLastReport = millis();
//  }
//}

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
 
#define REPORTING_PERIOD_MS 1000
 
// PulseOximeter is the higher level interface to the sensor
// it offers:
// * beat detection reporting
// * heart rate calculation
// * SpO2 (oxidation level) calculation
PulseOximeter pox;
 
uint32_t tsLastReport = 0;
 
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
Serial.println("Beat!");
}
 
void setup()
{
Serial.begin(115200);
 
Serial.print("Initializing pulse oximeter..");
 
// Initialize the PulseOximeter instance
// Failures are generally due to an improper I2C wiring, missing power supply
// or wrong target chip
if (!pox.begin()) {
Serial.println("FAILED");
for(;;);
} else {
Serial.println("SUCCESS");
}
 
// The default current for the IR LED is 50mA and it could be changed
// by uncommenting the following line. Check MAX30100_Registers.h for all the
// available options.
// pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
// Register a callback for the beat detection
pox.setOnBeatDetectedCallback(onBeatDetected);
}
 
void loop()
{
// Make sure to call update as fast as possible
pox.update();
 
// Asynchronously dump heart rate and oxidation levels to the serial
// For both, a value of 0 means "invalid"
if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
Serial.print("Heart rate:");
Serial.print(pox.getHeartRate());
Serial.print("bpm / SpO2:");
Serial.print(pox.getSpO2());
Serial.println("%");
 
tsLastReport = millis();
}
}
