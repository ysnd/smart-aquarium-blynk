#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME "Smart Fish Tank"
#define BLYNK_AUTH_TOKEN ""

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7

#include "BlynkEdgent.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
const int oneWireBus = 13;   
const int led = 14; 
const int heat = 27; 
int servoPin = 26;
Servo servol;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
float t;
SimpleTimer timer;
WidgetLCD lcd(V0);

void sendSensor()
{
  sensors.requestTemperatures(); 
  t = sensors.getTempCByIndex(0);
  if (t > 31) {
    digitalWrite(heat, HIGH);
    Serial.print("Suhu : "); 
    lcd.clear();
    lcd.print(0, 0, "Suhu : ");
    lcd.print(0, 1, t);    
    Serial.print(t);
    Serial.println("°C ");
    Serial.println("Suhu Panas mematikan Heater"); 
    lcd.clear();
    lcd.print(0, 0, "Suhu Panas"); 
    lcd.print(0, 1, "Matikan Heater"); 
    delay(1000);
    }

  else if (t < 27) {
    digitalWrite(heat, LOW);
    Serial.print("Suhu : ");
    lcd.clear();
    lcd.print(0, 0, "Suhu : ");
    lcd.print(0, 1, t);
    Serial.print(t);
    Serial.println("°C "); 
    Serial.println("Suhu Dingin menghidupkan Heater"); 
    lcd.clear();
    lcd.print(0, 0, "Suhu Dingin"); 
    lcd.print(0, 1, "hidupkan Heater");  
    delay(1000);
    }
  digitalWrite(led, V1);
  Blynk.virtualWrite(V4, t);
  Blynk.virtualWrite(V1, led);
  Blynk.virtualWrite(V2, heat);
}
void setup()
{
  Serial.begin(115200);
  delay(100);
  sensors.begin();
  lcd.print(0, 0, "Memulai");
  lcd.print(0, 1, "Membaca");
  delay(1000);
  lcd.clear();
  servol.attach(4);  
  pinMode(heat, OUTPUT);
  digitalWrite(heat, LOW);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);  
  BlynkEdgent.begin();
  servol.write(0);
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  BlynkEdgent.run();
  lcd.clear();
  lcd.print(0, 0,"Suhu :");
  lcd.print(0, 1, t);  
  lcd.clear();
  timer.run();
}

