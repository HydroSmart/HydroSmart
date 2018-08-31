#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <string>

#define TIME_TO_SLEEP_IN_MINUTES 30

using namespace std;

const char* ssid = "KalkNet";
const char* password =  "Pupikon1";

int channelID = 406588;
string ChannelWriteKey ="S8H1MHMDX5XP52LD";
unsigned long StartTime,EndTime;

int sensorPin = 33;
int sensorActivationPin = 17;
u16_t MoistVal = 0;
int WifiStrenghVal = 0;
void printValuesToSerial();

WiFiClient Client;

bool WaitWifiConnected()
{
    //wait for connection for 5 seconds
    Serial.println( "waiting for Connection to WiFi" );
    for(int i = 0 ; i < 50 && WiFi.status() != WL_CONNECTED; i++ )
        delay(100);
    
    if(WiFi.status() != WL_CONNECTED)
        return false;

    Serial.println( "Connected");  // Inform the serial monitor
    return true;
}

int GetWifiStrength(int Samples, int DelayInMS = 20)
{
    long averageRSSI=0;
    
    for (int i=0;i < Samples;i++)
    {
        averageRSSI += WiFi.RSSI();
        delay(DelayInMS);
    }

    return averageRSSI / Samples; 
}

void HTTPPost()
{   
  ThingSpeak.begin(Client); 
  ThingSpeak.setField(1, MoistVal );
  ThingSpeak.setField(2, WiFi.RSSI());
  EndTime = millis();
  ThingSpeak.setField(3, (long)(EndTime - StartTime));

  int writeSuccess = ThingSpeak.writeFields( channelID, ChannelWriteKey.c_str() );
  if(writeSuccess)
    Serial.println("Thinkspeak channel updated");
  else
    Serial.println("Thinkspeak channel update failed!");
}


void setup() {
  Serial.begin(9600);
  StartTime = millis();
  Serial.println("Starting up...");
  WiFi.begin(ssid, password);
  pinMode(sensorPin, INPUT);
  pinMode(sensorActivationPin, OUTPUT);
  digitalWrite(sensorActivationPin, HIGH);
}

void loop() {
  //digitalWrite(sensorActivationPin, HIGH);
  //delay(5000);
  
  if(WaitWifiConnected())
  {
    MoistVal = analogRead(sensorPin);
    HTTPPost();
  }
  else
    Serial.println("Wifi connection failed");

  printValuesToSerial();
  Serial.println("Going to sleep");
  const int minutes_to_microseconds = 60000000;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_IN_MINUTES * minutes_to_microseconds);
  esp_deep_sleep_start();
}

int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 465, 0, 100);
  return percentValue;
}

void printValuesToSerial()
{
  Serial.print("\n\nAnalog Value: ");
  Serial.println(MoistVal);
  //Serial.print("\nPercent: ");
  //Serial.print(percent);
  //Serial.print("%");
}