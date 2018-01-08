#include <IRremote.h>
#include <Adafruit_SleepyDog.h> 
#include <Adafruit_SSD1306.h>
#include "Wire.h"
#include "SPI.h"
#include <EEPROM.h>

#define OLED 0x3C
#define DS3231_I2C_ADDRESS 0x68
#define LED   13
#define OLED_RESET 10

#define HEAT  0xFF23DC
#define HEAT2 0x690CEFE0
#define TV    0x20DF10EF

IRsend irsend;
Adafruit_SSD1306 display(OLED_RESET);
int BUTTON = 9;
int SWITCH = 4;
int POT1 = A1;

void printTime();
void getTime();
String CalculateTime(int); 
void displayTime(String, String, String);

int B_dayOfMonth;
int B_month;
int B_year;
int B_dayOfWeek;
int switchState;
int buttonState;
int timeState;
int Hour;
int Min;
int EEpromAddr = 0;
int flag = 0;
int temp = 0; 

String B_minute;
String B_second;
String B_hour;
String Time;
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
   
  if (switchState == LOW){ // Manual Mode
    timeState = (0.9 * timeState + 0.1 * analogRead(POT1));
    target = Calculate(timeState);
    Serial.println("target is " + target); 

    temp = target.toInt(); 
    EEPROM.write(EEpromAddr, temp & 0xff);   //I'm not completly sure how these two lines work, but they set the address
    EEPROM.write(EEpromAddr+1, temp >> 8);   // up correctly to write my string to eeprom and make it so I can later retrieve it. 
    
    getTime();
    
    displayTime(B_hour, B_minute, target); 
    buttonState = digitalRead(BUTTON);

    if (buttonState == HIGH){     // If button is pressed, send the signal. 
         irsend.sendNEC(HEAT, 32); //send IR code
         delay(300);
    }
  }
  
  else{                     //Auto Mode
    digitalWrite(LED, LOW);
    display.clearDisplay(); 
    display.display();
    
    Watchdog.sleep();   // Sleep unit for 8 seconds. 
    delay(200); 
   
    getTime(); 
    
    check.concat(B_hour);
    check.concat(B_minute);
    temp = EEPROM.read(EEpromAddr) + (EEPROM.read(EEpromAddr+1) << 8);

    if(temp < 1000){ //check the value saved and see if it needs to add a leading zero. 
      target.concat("0");
    }
    target.concat(temp); 

    Serial.print("Target = ");
    Serial.println(target); 
  
    if((check == target) && (B_dayOfWeek < 6)){  //Check to see if day of week is Mon - Fri
      flag = 1;
      digitalWrite(LED, HIGH);
    }
   
    if(flag == 1){
      autoTurnOn();
      flag = 0;
    }
  
     check = ""; //reset the check variable.
     target = ""; //reset target so the concat operations don't ruin it later. 
     delay(200);
  }
}


String Calculate(int timeState){
  int T_hours;
  int T_min;
  String T_Time; 

  int NewVal = map(timeState, 0, 700, 0, 144);  //Map potentiometer (read from timeState), 0 to max pot value (700)
                                                // and map those values to 0 to 144 (The amount of 10 min intervals in a day)
  T_hours = (NewVal/6);
  T_min   = (NewVal%6); 

  if(T_hours < 10){ //add leading zero if time is less than 10:00
    T_Time.concat("0"); 
  }
  
  T_Time.concat(T_hours);
  T_Time.concat(T_min);
  T_Time.concat("0");
  
  return T_Time;
}


void displayTime(String B_hour, String B_minute, String target){
  display.clearDisplay(); 
  display.setCursor(10, 5);
  display.print("Time: ");
  display.print(B_hour);
  display.print(":");
  display.print(B_minute); 
  display.setCursor(10, 20);
  display.print("Set:  ");
  
  String C_hour = target;
  C_hour.remove(2, 2);
  
  String C_min  = target;
  C_min.remove(0, 2); 
  
  display.print(C_hour);
  display.print(":");
  display.print(C_min); 
  
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
