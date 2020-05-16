
#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SD.h> // for the SD card
#include <Wire.h>
#include <DS3231.h>

DS3231  rtc(SDA, SCL);

#define SS_PIN 10
#define RST_PIN 9
#define CS_SD 4
#define LED_PIN 2 
#define LED_PIN 3

float average;
float calibration_factor = -402.46;

const int tarebutton = 8;
const int buttonPin = 6;
const int buttonPin2 = 7; // the number of the pushbutton pin
int buttonState = 0;
int lastButtonState = 0;

// Create a file to store the data
File myFile;


MFRC522 mfrc522(SS_PIN, RST_PIN);

HX711 scale;
float elements; 

LiquidCrystal_I2C lcd(0x27,20,4);
String LCDLine1,LCDLine2;

int n;
float rounded;

void updateLCD () {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(LCDLine1);
  
  
  lcd.setCursor(0,1);
  lcd.print(LCDLine2);

}

void setup() {
  Serial.begin(9600);
  rtc.begin();
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode (buttonPin, INPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  lcd.init();
  lcd.backlight();
  
  LCDLine1="Welcome";
  LCDLine2="Scan your ID";
  updateLCD();

  
  if (SD.begin(CS_SD))
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }

  while(true){
  // Look for new cards02`11
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
      continue;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  { 
    
  }
    break;
  }
  
  void(* resetFunc) (void) = 0;
   
  while(true){
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
   for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "9E 7E 85 89") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    tone(5, 1000, 1000);
    digitalWrite(2, HIGH);
    delay(3000);
    digitalWrite(2, LOW);
    break;
  }
 
 else   {
    Serial.println(" Access denied");
    tone(5, 2000, 1000);
    digitalWrite(3, HIGH);
    delay(3000);
    digitalWrite(3, LOW);
    resetFunc();
  }
  }
  
  Serial.println("HX711 Demo");
  LCDLine1="Initializing ...";
  LCDLine2="";
  updateLCD();

  
  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT  - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(A1,A0);
  scale.set_scale(calibration_factor);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0
            // by the SCALE parameter set with set_scale
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  Serial.println("Readings:");
}




void itemweight()
{
          
        elements = scale.get_units(15);
        Serial.print(" \t| Weight \t");
        Serial.print(elements);
    
  
}

void Scale() 
  { 
     
    scale.set_scale(calibration_factor);
    average = scale.get_units(20);
if (average < 0)
{
average = 0.00;
}
 

  float number =  average/elements;//divide the average of 20 readings with the elements value to get the number of elements
  n = round(number);
  // n = (int)number; // convert float into int
   
  Serial.print("\t| elements:\t");
  Serial.print(n); //serial monitor
  Serial.print("\t| average:\t");
  Serial.println(average);
  // lcd.clear();
       //set cursor first row
  LCDLine1=String(average)+" g"; //print on lcd average value from 10 raw readings 1 time
  LCDLine2=String(n)+" pcs";
  updateLCD();
 // serial monitor -||-
  
 
   //print PCS
  
  scale.power_down();             // put the ADC in sleep mode
  delay(1000);
  scale.power_up();
  updateLCD();
  } 
  
unsigned long getID(){
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}
void buttontare()
{
  buttonState = digitalRead(tarebutton);
  if (buttonState == HIGH){
    scale.tare();
  LCDLine1="Taring...";
  LCDLine2="";
  delay(1500);
  updateLCD();
  }
}

void myFiles()
{
  buttonState = digitalRead(buttonPin2);
  if (buttonState == HIGH) {
  myFile = SD.open("opit8.txt", FILE_WRITE);
  myFile.println();
  myFile.println("User ID:");
  myFile.print(getID());
  myFile.println();
  myFile.println("Day:");
  myFile.print(rtc.getDOWStr());
  myFile.println();
  myFile.println("Date:");
  myFile.print(rtc.getDateStr());
  myFile.println();
  myFile.println("Time:");
  myFile.print(rtc.getTimeStr());
    Serial.print("Writing to test.txt...");
    myFile.println();
    myFile.println("PCS:");
    myFile.print(n);
    myFile.close();
    LCDLine1="Saving...";
  LCDLine2="";
  delay(1500);
  updateLCD();
  }
  }



void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    LCDLine1="Place one item"; 
    LCDLine2=String(elements,1) + "g";
    updateLCD(); 
    itemweight();
  }
  else {
       buttontare();
       Scale();
       myFiles();
       updateLCD();
   
  }
 
}




 
  
