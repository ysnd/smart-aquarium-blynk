#define BLYNK_TEMPLATE_ID           ""
#define BLYNK_TEMPLATE_NAME         "SmartTank"
#define BLYNK_AUTH_TOKEN            ""
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <ESP32Servo.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
const int oneWireBus = 13;
const int led = 12; 
const int heat = 14; 
int servoPin = 27;
int heater_status;
float Vref = 3.3;
float ec = 0;
unsigned int tds = 0;
float t = 0;
float ecCalibration = 1;
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";
char auth[] = BLYNK_AUTH_TOKEN;
const byte tds_pin = A0;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);


BLYNK_WRITE(V5)
{
  servo.write(45);
  delay(500);
  servo.write(0);
  Blynk.logEvent("pakan", "Pakan sudah diberi");
}
BLYNK_WRITE(V4)
{
  int pinValue=param.asInt();
  digitalWrite(led,pinValue);
  Blynk.logEvent("led", "lampu dihidupkan");
  }
BLYNK_WRITE(V3)
{
  int pinValue=param.asInt();
  digitalWrite(heat,pinValue);
  Blynk.logEvent("heater", "heater dihidupkan");  
}


void setup() {
  Serial.begin(115200); // Dubugging on hardware Serial 0
  lcd.begin();       
  lcd.backlight();
  Serial.println(" Memulai Smart ");
  lcd.setCursor(0,0);
  lcd.print("    Aquarium    ");
  delay(800);
  lcd.clear();
  Blynk.begin(auth, ssid, pass);
  Serial.println("Menghubungkan WiFi ke SSID");
  lcd.setCursor(0,0);
  lcd.print(" Menghubungkan ");
  lcd.setCursor(0,1);
  lcd.print("      WiFi      ");
  delay(800);
  lcd.clear();
  sensors.begin();
  servo.attach(servoPin);
  servo.write(0);
  pinMode(led, OUTPUT);
  pinMode(heat, OUTPUT);
}

void loop() {
  Blynk.run();
  readTdsQuick();
  delay(1000);
}

void readTdsQuick() {
  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
  float rawEc = analogRead(tds_pin) * Vref / 4096.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float temperatureCoefficient = 1.0 + 0.02 * (t - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  ec = (rawEc / temperatureCoefficient) * ecCalibration; // temperature and calibration compensation
  tds = (133.42 * pow(ec, 3) - 255.86 * ec * ec + 857.39 * ec) * 0.5; //convert voltage value to tds value
  Serial.print(F("TDS:")); Serial.println(tds);
  Serial.print(F("EC:")); Serial.println(ec, 2);
  Serial.print(F("Temperature:")); Serial.println(t,2);
  lcd.setCursor(0,0);     
  lcd.print("Suhu : ");
  lcd.print(t);   
  lcd.print(" ");          
  lcd.print(char(223));
  lcd.print("C");
  lcd.setCursor(0,1); 
  lcd.print("TDS  : ");
  lcd.print(tds);           
  lcd.print(" ppm");
  
  if (t >= 32) {
    digitalWrite(heat, LOW);
    heater_status = 0;
    Serial.print("Suhu terlalu panas mematikan Heater");
    Blynk.logEvent("suhu_tinggi","Suhu Terlalu Panas Mematikan Heater");
  }
  else if (t <= 22) {
    digitalWrite(heat, HIGH);
    heater_status = 1;
    Serial.print("Suhu terlalu dingin menghidupkan Heater");
    Blynk.logEvent("suhu_rendah","Suhu Terlalu Dingin Menghidupkan Heater");
  }
  else if (tds >= 400) {
    Blynk.logEvent("TDS_tinggi","Jumlah Zat Padat yang Terlarut dalam aquarium sangat tinggi, Segeralah mengganti air aquarium");
    Serial.print("Jumlah Zat Padat yang Terlarut dalam aquarium sangat tinggi, Segeralah mengganti air aquarium");
  }
  Blynk.virtualWrite(V0,(tds));
  Blynk.virtualWrite(V2,(ec));
  Blynk.virtualWrite(V1,(t));
  Blynk.virtualWrite(V3,(heater_status));
}
