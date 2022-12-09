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


///////////// pin numbers - /////////////////
int pulseSCL = 22;  // check whether it works - previously in 32
int pulseSDA = 21;  // check whether it works - previously in 33
int musclePin = 35;
//int LCDpin = ??????;





////////////////GLOBAL VARIABLES /////////////////////

int heartRate = 72; // default value at the beginning
int saturationValue = 97; // default value for saturation at the beginning
float muscleSensor_Reading = 3420;  // i have no idea what this value could mean
uint32_t tsLastReport = 0;

int muscleCount = 0;
int heartCount = 0;
int oxyCount = 0;

const char* server = "mqtt3.thingspeak.com"; //Change
char mqttUserName[] = "sreyas";
char mqttPass[] = "MBWXD2NHZPU55XON"; // API
long channelID = 1758260;
char writeAPI[] = "TJ1CSTX0ZQB9YL4H";
char ssid[] = "LessGo";
char password[] = "12345678";


//////////////////////////////////////////////////////


/////////////////// INITIALISING HEADERS ///////////////
PulseOximeter pox;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClient client;
PubSubClient mqttClient(server, 1883, client);

////////////////////////////////////////////////////////

 
void onBeatDetected()
{
    Serial.println("Beat!");
}


void setup()
{
    Serial.begin(9600);


    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    delay(2000);
    display.clearDisplay();
  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.println("Scrolling Hello");
    display.display(); 
    delay(100);
    
    
    Serial.print("Initializing pulse oximeter..");
 
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
 
    pox.setOnBeatDetectedCallback(onBeatDetected);

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

 
int heartRateValue()
{
    pox.update();
    int heartRate_Reading;
    
    heartRate_Reading = pox.getHeartRate(); // heartRate_Reading -> stores the value read from the sensor


    if (heartRate_Reading < 50)
    {
      return 0;
    }
    
    if (heartRate_Reading > 180) // if the observed value of heartRate is out of range, use the previous value
    {
      return heartRate; // heartRate -> global variable storing the previous fair value of hear rate
    }
    tsLastReport = millis();
    return heartRate_Reading;
}

int oxygenValue()
{
    pox.update();
    int spo2_Reading;
    
    spo2_Reading = pox.getSpO2(); // heartRate_Reading -> stores the value read from the sensor

    if (spo2_Reading < 85)  // if sensor gives absurdly low value, it should print 0
    {
      return 0;
    }
    
    if (spo2_Reading > 100) // if the observed value of heartRate is out of range, use the previous value
    {
      return saturationValue; // saturationValue -> global variable storing the previous fair value of O2 saturation
    }

    tsLastReport = millis();
    return spo2_Reading;
}

float muscleSensorReading() 
{
  float sensorValue = analogRead(32);
  float millivolt = (sensorValue/1023)*5;
  return  millivolt;  // the value of the muscle sensor
}


void mqttPublish(long pubChannelID, char* pubWriteAPIKey, String field, float value)
{
  String dataString = field + "=" + String(value);
  Serial.println(dataString);
  String topicString = "channels/" + String(pubChannelID) + "/publish";
  mqttClient.publish(topicString.c_str(),dataString.c_str());
  Serial.println(pubChannelID);
  Serial.println("");
}


void loop() {
  while(!mqttClient.connected())
  {
    Serial.println("Connect Loop");
    Serial.println(mqttClient.connect("CjMsGzEVCgQrEDcfHBo4GTM","CjMsGzEVCgQrEDcfHBo4GTM","sYGBcuTIqXFk0ynJewd+HACJ"));
    Serial.println(mqttClient.connected());
//    mqttConnect();
  }
//  Serial.println("MQTT Connected");
  mqttClient.loop();

  saturationValue = oxygenValue();
  heartRate = heartRateValue();
  muscleSensor_Reading = muscleSensorReading();
  
  if (oxyCount >= 500) // originally >= 500
  {
    mqttPublish(channelID, writeAPI, "field1", saturationValue); // field v= "field1"/"field2"/...  
    oxyCount = 0;
  }

  if (heartCount >= 100)  // originally >= 100
  {
     mqttPublish(channelID, writeAPI, "field2", heartRate); // field v= "field1"/"field2"/...  
     heartCount = 0;
  }

  if (muscleCount >= 100) // originally >= 100
  {
     mqttPublish(channelID, writeAPI, "field3", muscleSensor_Reading); // field v= "field1"/"field2"/...  
     muscleCount = 0; 
  }

  muscleCount++;
  heartCount++;
  oxyCount++;
  
  delay(10);
}
