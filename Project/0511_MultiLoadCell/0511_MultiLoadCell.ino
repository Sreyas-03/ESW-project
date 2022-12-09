#include <Arduino.h>
#include "HX711.h"


#include <ThingSpeak.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


////////////////////////////////////////////////////
const char* server = "mqtt3.thingspeak.com"; //Change
char mqttUserName[] = "sreyas";
char mqttPass[] = "MBWXD2NHZPU55XON"; // API
long channelID = 1758260;
char writeAPI[] = "TJ1CSTX0ZQB9YL4H";
char ssid[] = "LessGo";
char password[] = "12345678";

WiFiClient client;
PubSubClient mqttClient(server, 1883, client);
///////////////////////////////////////////////////


///////////// HX711 circuit wiring /////////////////
//const int LOADCELL1_DOUT_PIN = 19;
//const int LOADCELL1_SCK_PIN = 18;
//
//const int LOADCELL2_DOUT_PIN = 5;
//const int LOADCELL2_SCK_PIN = 23;
//
const int LOADCELL3_DOUT_PIN = 13;
const int LOADCELL3_SCK_PIN = 35;

const int LOADCELL4_DOUT_PIN = 22;
const int LOADCELL4_SCK_PIN = 23;

//////////////// ABOVE IS FOR ESP ////////////////

//const int LOADCELL1_DOUT_PIN = 2; // either these two or the two above
//const int LOADCELL1_SCK_PIN = 3;
//
//const int LOADCELL2_DOUT_PIN = 4;
//const int LOADCELL2_SCK_PIN = 5;

//const int LOADCELL3_DOUT_PIN = 6; // originall 12
//const int LOADCELL3_SCK_PIN = 7; // originall 13
//
//const int LOADCELL4_DOUT_PIN = 8;
//const int LOADCELL4_SCK_PIN = 9;

///////////////// THIS IS FOR ARDUINO //////////////////

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

void setup_LoadCell(HX711* inpScale, int LOADCELL_DOUT_PIN, int LOADCELL_SCK_PIN)
{
  //  HX711 scale = *inpScale;
  (*inpScale).begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println((*inpScale).read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println((*inpScale).read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println((*inpScale).get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println((*inpScale).get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  // by the SCALE parameter (not set yet)

  (*inpScale).set_scale(-459.542);
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  (*inpScale).tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println((*inpScale).read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println((*inpScale).read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println((*inpScale).get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println((*inpScale).get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
  // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}


void mqttPublish(long pubChannelID, char* pubWriteAPIKey, String field, float value)
{
  String dataString = field + "=" + String(value);
  Serial.println(dataString);
  String topicString = "channels/" + String(pubChannelID) + "/publish";
  mqttClient.publish(topicString.c_str(), dataString.c_str());
  Serial.println(pubChannelID);
  Serial.println("");
}





void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  //  setup_LoadCell(&scale1, LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  //  setup_LoadCell(&scale2, LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  setup_LoadCell(&scale3, LOADCELL3_DOUT_PIN, LOADCELL3_SCK_PIN);
  setup_LoadCell(&scale4, LOADCELL4_DOUT_PIN, LOADCELL4_SCK_PIN);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(server, 1883);
  ThingSpeak.begin(client);
}

float loadCell_Value(int number, HX711 scale)
{
  Serial.print("one reading:\t");
  float reading = scale.get_units();
  reading = max(reading, -reading) / 10;
  if (number == 3)
  {
    if (reading > 1.2 && reading < 1.8)
      reading = 0;
  }
  reading = reading / 1.2;
  Serial.print(reading, 1);
  Serial.print("kg\t| average:\t");
  float avg = scale.get_units(10);
  avg = max(avg, -avg) / 10;
  Serial.print(avg, 5);
  Serial.println("kg");
  return reading; // returns the load cell value
}


void loop()
{
  while (!mqttClient.connected())
  {
    Serial.println("Connect Loop");
    Serial.println(mqttClient.connect("CjMsGzEVCgQrEDcfHBo4GTM", "CjMsGzEVCgQrEDcfHBo4GTM", "sYGBcuTIqXFk0ynJewd+HACJ"));
    Serial.println(mqttClient.connected());
    //    mqttConnect();
  }
  //  Serial.println("MQTT Connected");
  mqttClient.loop();

  //  loadCell_Value(1, scale1);
  //  loadCell_Value(2, scale2);
  float LC3 = loadCell_Value(3, scale3);
  float LC4 = loadCell_Value(4, scale4);
  mqttPublish(channelID, writeAPI, "field4", LC3);
  mqttPublish(channelID, writeAPI, "field5", LC4);
  delay(5000);
}
