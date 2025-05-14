int ledRedPin = 13;
int ledYellowPin = 12;
int ledGreenPin = 11;
int ldrPhotoSensor = A0;
int boozerPin = 8;
unsigned long boozerStartTime = 0;
bool boozerActive = false;
float ultimasTemperaturas[5] = {0};
int indiceTemp = 0;
float ultimasUmidades[5]= {0};
int indiceUmi = 0;
bool mediaCalculada = false;
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define col 16
#define lin  2
#define ende  0x27
LiquidCrystal_I2C lcd(ende, col, lin);

byte shit[8] = {
  B00000,
  B00011,
  B00101,
  B01001,
  B10001,
  B10001,
  B01011,
};

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE DHT22
//#define DHTTYPE DHT11
#define DHTPIN 2
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


void setup() {

  pinMode(ledRedPin, OUTPUT);
  pinMode(ledYellowPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(boozerPin, OUTPUT);
  Serial.begin(115200);
  dht.begin();

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(sensor.name);
  Serial.print(sensor.max_value);
  Serial.print(sensor.min_value);
  Serial.print(sensor.resolution);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Display Testing!");
  lcd.setCursor(0, 1);
  lcd.print("Arduino LCD");
  lcd.createChar(0, shit);
  lcd.setCursor(15, 1);  
  lcd.write(byte(0));
  dht.humidity().getSensor(&sensor);
  Serial.println(sensor.name);
  Serial.print(sensor.max_value);
  Serial.print(sensor.min_value);
  Serial.print(sensor.resolution);
  delayMS = sensor.min_delay / 1000;
  delay(1000);
}

void loop() {
  // Valores do led
  int ldrValue = analogRead(ldrPhotoSensor);
  Serial.println(ldrValue); Serial.println("Luz em lux");
  unsigned long currentTime = millis();
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledYellowPin, LOW);
  digitalWrite(ledRedPin, LOW);
  if (ldrValue >= 0 && ldrValue <= 300) {
    digitalWrite(ledGreenPin, HIGH);
    noTone(boozerPin);
    boozerActive = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Luz boa");
    delay(3000);
    lcd.clear();
  }
  else if (ldrValue >= 301 && ldrValue <= 700) {
    digitalWrite(ledYellowPin, HIGH);
    noTone(boozerPin);
    boozerActive = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ambiente a meia");
    lcd.setCursor(0, 1);
    lcd.print("luz");
    delay(3000);
    lcd.clear();
  }
  else if (ldrValue > 701) {
    digitalWrite(ledRedPin, HIGH);
    if (!boozerActive) {
      tone(boozerPin, 262, currentTime);
      boozerStartTime = currentTime;
      boozerActive = true;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ambiente muito");
    lcd.setCursor(0, 1);
    lcd.print("CLARO");
    delay(3000);
    lcd.clear();
  }
  else {
    noTone(boozerPin);
    boozerActive = false;
  }

  //DHT
  sensors_event_t tempEvent, humidEvent;
  dht.temperature().getEvent(&tempEvent);
  dht.humidity().getEvent(&humidEvent);

    if (isnan(tempEvent.temperature)) {
    Serial.println("Erro na leitura da Temperatura!");
  }

    else {
    Serial.print(tempEvent.temperature);
    Serial.println(" Temperatura em C ");
    
  }
      if (isnan(humidEvent.relative_humidity)) {
    Serial.println("Erro na leitura da Umidade!");
  } 
    else {
    Serial.print(humidEvent.relative_humidity);
    Serial.println(" Umidade em % ");
    
    }

  // Pega as ultimas 5 temperaturas e faz a média
  ultimasTemperaturas[indiceTemp] = tempEvent.temperature;
  indiceTemp = (indiceTemp + 1) %5 ;
  float mediaTemperatura = (
  ultimasTemperaturas[0] + ultimasTemperaturas[1] + ultimasTemperaturas[2] + ultimasTemperaturas[3] + ultimasTemperaturas[4]
  ) / 5.0;

  // Pega as ultimas 5 umidades e faz a média
  ultimasUmidades[indiceUmi] = humidEvent.relative_humidity;
  indiceUmi = (indiceUmi + 1) %5;
  float mediaUmi = (
  ultimasUmidades[0] + ultimasUmidades[1] + ultimasUmidades[2] + ultimasUmidades[3] + ultimasUmidades[4]
  ) / 5.0;
  if (indiceTemp == 0 && indiceUmi == 0) {
  mediaCalculada = true;
  }

  // Valores da temperatura
  if(mediaCalculada){
      //temperatura
    if (mediaTemperatura >= 10 && mediaTemperatura <= 15) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp. OK: ");
      lcd.setCursor(0,1);
      lcd.print("Temp. = ");
      lcd.print(mediaTemperatura);
      lcd.print("C");
      delay(3000);
      lcd.clear();
    }
    else if(mediaTemperatura > 15) {
      digitalWrite(ledYellowPin, HIGH);
      if (!boozerActive) {
        tone(boozerPin, 262, currentTime);
        boozerStartTime = currentTime;
        boozerActive = true;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp. Alta: ");
      lcd.setCursor(0,1);
      lcd.print("Temp. = ");
      lcd.print(mediaTemperatura);
      lcd.print("C");
      delay(3000);
      lcd.clear();
    }
  else if(mediaTemperatura < 10) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp. Baixa: ");
      lcd.setCursor(0,1);
      lcd.print("Temp. = ");
      lcd.print(mediaTemperatura);
      lcd.print("C");
      delay(3000);
      lcd.clear();
    }

    //umidade
    // Valores da umidade
    if (mediaUmi >= 50 && mediaUmi <= 70) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Umidade OK: ");
      lcd.setCursor(0,1);
      lcd.print("Umidade = ");
      lcd.print(mediaUmi);
      lcd.print("%");
      delay(3000);
      lcd.clear();
    }
    else if (mediaUmi > 70) {
      digitalWrite(ledRedPin, HIGH);
      if (!boozerActive) {
        tone(boozerPin, 262, currentTime);
        boozerStartTime = currentTime;
        boozerActive = true;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Umidade Alta: ");
      lcd.setCursor(0,1);
      lcd.print("Umidade = ");
      lcd.print(mediaUmi);
      lcd.print("%");
      delay(3000);
      lcd.clear();
    }
    else if (mediaUmi < 50) {
      digitalWrite(ledRedPin, HIGH);
      if (!boozerActive) {
        tone(boozerPin, 262, currentTime);
        boozerStartTime = currentTime;
        boozerActive = true;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Umidade Baixa: ");
      lcd.setCursor(0,1);
      lcd.print("Umidade = ");
      lcd.print(mediaUmi);
      lcd.print("%");
      delay(3000);
      lcd.clear();
    }
  delay(200);

  }
}