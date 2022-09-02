#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "testmodel.h"
#include <String.h>
Eloquent::ML::Port::RandomForest classifier;
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mod(7,6);

#define RE 9
#define DE 8

//const byte code[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD};
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
const byte moist[] = {0x01, 0x03, 0x00, 0x12, 0x00, 0x01,0x25,0xCA};
const byte ph[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0B};
const byte temp[] = {0x01, 0x03, 0x04, 0xFF, 0x9B, 0x5A, 0x3D};
const byte hum[] = {0x01, 0x03, 0x04, 0x02, 0x92, 0x5A, 0x3D};
byte values[11];
int cropprediction;
const int chipSelect = 4;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.backlight();
  mod.begin(9600);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
//  classify();

  //SDcard
    Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void loop() {
  // put your main code here, to run repeatedly:
  byte soilNitrogen = nitrogen();
  delay(1000);
  byte  soilPhosphorus = phosphorous();
  delay(1000);
  byte  soilPotassium = potassium();
  delay(1000);
  byte  soilMoisture=moisture();
  delay(1000);
  byte  soilPh=soilph();
  delay(1000);
  byte  soilTemperature=temperature();
  delay(1000);
  byte  soilHumidity=humidity();
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil parameters: ");
  
  lcd.setCursor(0, 1);
  lcd.print("PH: ");
  lcd.setCursor(10, 1);
  lcd.print(soilPh);
  Serial.print(soilPh);
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Nitrogen: ");
  lcd.setCursor(10, 1);
  lcd.print(soilNitrogen);
  Serial.print(soilNitrogen);
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Phosphorous: ");
  lcd.setCursor(10, 1);
  lcd.print(soilPhosphorus);
  Serial.print(soilPhosphorus);
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Potassium: ");
  lcd.setCursor(10, 1);
  lcd.print(soilPotassium);
  Serial.print(soilPotassium);
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Moisture: ");
  lcd.setCursor(10, 1);
  lcd.print(soilMoisture);
  Serial.print(soilMoisture);
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.setCursor(10, 1);
  lcd.print(soilHumidity);
  Serial.print(soilHumidity);
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("Temperature: ");
  lcd.setCursor(12, 1);
  lcd.print(soilTemperature);
  Serial.print(soilTemperature);
  delay(1000);

  //Machine learning model
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Predicting....");
  delay(5000);
  float x_sample[] = {0.66176942,-0.86930503,-0.16539138,0.42064859};
  cropprediction = classifier.predict(x_sample);
   if(cropprediction == 3){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Crop prediction:");
      lcd.setCursor(0, 1);
      lcd.print("Forage crops");
    }
    else if(cropprediction == 4){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Crop prediction:");
      lcd.setCursor(0, 1);
      lcd.print("Cereals");
      }
     else if(cropprediction == 0){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Crop prediction:");
      lcd.setCursor(0, 1);
      lcd.print("Fruits");
      }
     else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Crop prediction:");
      lcd.setCursor(0, 1);
      lcd.print("Vegetable");
      }

      //SDcard
       File dataFile = SD.open("datalog.txt", FILE_WRITE);

      // if the file is available, write to it:
      if (dataFile) {
      dataFile.print(soilNitrogen);
      dataFile.print(",");
      dataFile.print(soilPhosphorus);
      dataFile.print(",");
      dataFile.print(soilPotassium);
      dataFile.print(",");
      dataFile.print(soilMoisture);
      dataFile.print(",");
      dataFile.print(soilPh);
      dataFile.print(",");
      dataFile.print(soilTemperature);
      dataFile.print(",");
      dataFile.print(soilHumidity);
      dataFile.print(",");
      dataFile.println(cropprediction);
      dataFile.close();
      // print to the serial port too:
      Serial.println(soilMoisture);
      }
      // if the file isn't open, pop up an error:
      else {
      Serial.println("error opening datalog.txt");
      }
           

      

  //GSM




  
}


//  ml model function
void classify() {
//    float x_sample[] = {-0.63282436,-0.63282436,  -1.11509086,-1.12063882};
//    float x_sample[] = {-0.38775857,0.5442014,  1.52192627,-1.37609069};
//    float x_sample[] = {-1.13306721,0.3357475,  -0.02300747,0.1565187};
      float x_sample[] = {0.66176942,-0.86930503,  -0.16539138,0.42064859};

    Serial.print("Predicted class: ");
    Serial.print(classifier.predict(x_sample));
    lcd.setCursor(0, 0);
    lcd.print("Predicted class: ");
    lcd.setCursor(0, 1);
    lcd.print(classifier.predict(x_sample));
}

//npk sensor
byte nitrogen(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(nitro,sizeof(nitro))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(phos,sizeof(phos))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(pota,sizeof(pota))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
  
byte moisture(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(moist,sizeof(moist))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
  
byte soilph(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(ph,sizeof(ph))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte temperature(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(temp,sizeof(temp))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte humidity(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(hum,sizeof(hum))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
