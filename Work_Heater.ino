#include <IRremote.h>
#include <Adafruit_SleepyDog.h> 
#include <Adafruit_SSD1306.h>
#include "Wire.h"
#include "SPI.h"

#define OLED 0x3C
#define DS3231_I2C_ADDRESS 0x68
#define LED   13 //use to be 5 for some reason.....
#define OLED_RESET 10

#define HEAT  0xFF23DC
#define HEAT2 0x690CEFE0
#define TV    0x20DF10EF

IRsend irsend;
Adafruit_SSD1306 display(OLED_RESET);
int BUTTON = 9;
int SWITCH = 12;
int POT1 = A7;
int POT2 = A2;

void printTime();
void getTime();
int getTarget();
String computeTarget(int, int);
void displayTime(String, String, String);

String B_minute;
String B_second;
String B_hour; 
int B_dayOfMonth;
int B_month;
int B_year;

int switchState;
int buttonState;
int HourState;
int MinState; 
int Hour;
int Min; 

int flag = 0;
String check  = ""; 
String target = "";

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
    HourState = analogRead(POT1); 
    MinState = analogRead(POT2);
    
    Hour = map(HourState, 0, 1020, 0, 23);
    Min = map(MinState, 0, 1020, 0, 59);
    target = computeTarget(Hour, Min); 
    
    getTime(); 
    displayTime(B_hour, B_minute, target); 
    buttonState = digitalRead(BUTTON);

    if (buttonState == HIGH){
         irsend.sendNEC(HEAT, 32); //send IR code
         delay(300);
    }
  }
  
  else{                 //Inside Sleep loop
    digitalWrite(LED, LOW);
    display.clearDisplay(); 
    display.display();
    
    Watchdog.sleep();
    delay(200); 
   
    getTime();
    getTarget();
   //printTime();
    
    check.concat(B_hour);
    check.concat(B_minute);
  
    Serial.print("Target = ");
    Serial.println(target); 
    Serial.print("Check = ");
    Serial.println(check); 
  
    if(check == target){
      //Serial.println("Target Met");
      flag = 1;
      digitalWrite(LED, HIGH);
    }
  
  
    if(flag == 1){
      autoTurnOn();
      flag = 0;
    }
  
     check = ""; //reset the check variable.
     //Serial.print("flag = ");
     //Serial.println(flag); 
     delay(200);
  }
}


int getTarget(){
    HourState = analogRead(POT1); 
    MinState = analogRead(POT2);
    
    Hour = map(HourState, 0, 1020, 0, 23);
    Min = map(MinState, 0, 1020, 0, 59);
    target = computeTarget(Hour, Min);   
    target.remove(2,1); //remove the : 

    return target.toInt();
}


String computeTarget(int pot1, int pot2){
  String myTime;
  int int_myHour = pot1;
  int int_myMin = pot2; 
  
  String myHour = String(pot1);
  String myMin = String(pot2);

  if(int_myHour < 10){
    myTime.concat("0");
  }
  myTime.concat(myHour); 
  myTime.concat(":");

  if(int_myMin < 10){
    myTime.concat("0");
  }
  myTime.concat(myMin); 

  return myTime;
  //Serial.println(myTime);
}

void displayTime(String B_hour, String B_minute, String target){
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
}


void autoTurnOn(){
  delay(60010); //wait for just over a minute to ensure the flag can't be set again.
  digitalWrite(LED, LOW); 
  delay(200);
    irsend.sendNEC(TV, 32); //send IR code
    delay(100); 
    irsend.sendNEC(HEAT, 32); //send IR code
  digitalWrite(LED, HIGH);   
  delay(100);
  digitalWrite(LED, LOW);
  delay(300);
}

