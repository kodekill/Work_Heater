#include <IRremote.h>
#include <Adafruit_SleepyDog.h> 
#include <Adafruit_SSD1306.h>
#include "Wire.h"
#include "SPI.h"

#define OLED 0x3C
//#define OLED 0x57
#define DS3231_I2C_ADDRESS 0x68
#define LED   5
#define OLED_RESET 10

#define HEAT  0xFF23DC
#define HEAT2 0x690CEFE0
#define TV    0x20DF10EF

IRsend irsend;
Adafruit_SSD1306 display(OLED_RESET);
int BUTTON = 9;
int SWITCH = 12;

void printTime();
void getTime();
String B_minute;
String B_second;
String B_hour; 
int B_dayOfMonth;
int B_month;
int B_year;
int buttonState;
int switchState;

int flag = 0;
String check  = ""; 
String target = "0650";

void setup(){

  Wire.begin();
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LED, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, OLED);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.setTextSize(.4);
  display.setTextColor(WHITE);
}


void loop() {
  switchState = digitalRead(SWITCH); 
   
  if (switchState == HIGH){
    getTime(); 
    Serial.println("HIGH");
    digitalWrite(LED, HIGH);
    
    display.clearDisplay(); 

    display.setCursor(0, 5);
    display.print("Time: ");
    display.print(B_hour);
    display.print(":");
    display.print(B_minute); 

    display.setCursor(0, 20);
    display.print("Set:  ");
    display.print(target);
    
    display.display();

    buttonState = digitalRead(BUTTON);

    if (buttonState == HIGH){
         Serial.println("Sending IR Code"); 
         irsend.sendNEC(HEAT, 32); //send IR code
         delay(300);
    }
    
  }
  else{
    Serial.println("LOW");
    digitalWrite(LED, LOW);
    display.clearDisplay(); 
    display.display();
    
    Watchdog.sleep();
    delay(200); 
   
    getTime(); 
   //printTime();
    
    check.concat(B_hour);
    check.concat(B_minute);
  
  
    Serial.print("Target = ");
    Serial.println(target); 
    Serial.print("Check = ");
    Serial.println(check); 
  
    if(check == target){
      Serial.println("Target Met");
      flag = 1;
      digitalWrite(LED, HIGH);
    }
  
  
    if(flag == 1){
      delay(60010); //wait for just over a minuet to ensure the flag can't be set again.
      irsend.sendNEC(HEAT, 32); //send IR code
      delay(300);
      //digitalWrite(LED, LOW); 
      flag = 0;
    }
  
     check = ""; //reset the check variable.
  
     Serial.print("flag = ");
     Serial.println(flag); 
     delay(200);
  }
}








