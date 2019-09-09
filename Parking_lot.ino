/*
	automatic parking lot with lights indicating available
	parking spots. no car is allowed to go in if there is 
	no available spot.
*/


#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>
#include <Servo.h>

// define pins
#define S1 A1
#define S2 A2
#define S3 A3
#define S4 A6
#define S5 A7
#define SOUT A0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define G1 6
#define G2 7
#define G3 8
#define G4 9
#define G5 10


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3f for a 16 chars and 2 line display
Ultrasonic ultrasonic(13, 12);	//trig,echo
Servo myservo;    
bool flag1=1, flag2=1;

int distance, space;

void setup() {
  lcd.init();    // initialize the lcd 
   // Print a message to the LCD.
  lcd.backlight();
  
  space=5;
  lcd.setCursor(3,0);
  lcd.print("Welcome to");
  lcd.setCursor(2,1);
  lcd.print("auto Parking!");
  myservo.attach(11);  // attaches the servo on pin 11 to the servo object
  
  //setup pinmodes
  for(int i=0;i<11;i++)
  pinMode(i,OUTPUT);
  pinMode(SOUT,INPUT);
  pinMode(S1,INPUT);
  pinMode(S2,INPUT);
  pinMode(S3,INPUT);
  pinMode(S4,INPUT);
  pinMode(S5,INPUT);
  
   myservo.write(90);
   delay(1000);
   myservo.detach();
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("available space");
   lcd.setCursor(0,1);
   lcd.print("in lot is:");
}


//checks if a car is going in, opens when car reaches a distance of 9cm, stays open
//for an extra 2 seconds after the ultrasonic can no longer see the car
//there is a delay of 2 seconds between the first car going in and the second car.
//available space is decreases by 1
void carin() { 
  distance = ultrasonic.read();
  
  if(distance <=9 && space >0 && flag1){
    myservo.attach(11);
    myservo.write(180);
    delay(1000);
    myservo.detach();
    flag1=0;                                                                                               
  }
  else if(distance >9 && space > 0 && !flag1)
  {
    delay(2000);
    myservo.attach(11);
    myservo.write(90);
    space--; 
    lcd.setCursor(11,1);
    lcd.print(space);
    delay(2000);
    myservo.detach();
    flag1=1; 
  }

}

//after a car steps on sensor, it assumes a car went out,
//increasing available space by one
void carout() 
{   
  int s = digitalRead(SOUT);
  if(s==0 && space < 5 )
  {

    if(flag2) flag2 = 0; 
  }
  else if(s==1 && !flag2)
  {
    flag2=1;
    space++;

  }
}

//checks every sensor to see if a car is parked there
//if it is light red, if it isnt light green
void sen(){
    if(analogRead(S1)<100){
    digitalWrite(R1,HIGH); 
    digitalWrite(G1,LOW);
    }
   else {
    digitalWrite(R1,LOW);
    digitalWrite(G1,HIGH); 
  }

  
   if(analogRead(S2)<100){
    digitalWrite(R2,HIGH); 
    digitalWrite(G2,LOW);
    }
   else {
    digitalWrite(R2,LOW);
    digitalWrite(G2,HIGH); 
  } 

  
  if(analogRead(S3)<100){
    digitalWrite(R3,HIGH); 
    digitalWrite(G3,LOW);
    }
   else {
    digitalWrite(R3,LOW);
    digitalWrite(G3,HIGH); 
  } 
  
  
  if(analogRead(S4)<100){
    digitalWrite(R4,HIGH); 
    digitalWrite(G4,LOW);
    }
   else {
    digitalWrite(R4,LOW);
    digitalWrite(G4,HIGH); 
  }
  
  
  if(analogRead(S5)<100){
    digitalWrite(R5,HIGH);
    digitalWrite(G5,LOW);
    }
   else {
    digitalWrite(R5,LOW);
    digitalWrite(G5,HIGH); 
  }
}

//print available space, check if a car is going in or out,
// update lights of parking spots
void loop() {
  lcd.setCursor(11,1);
  lcd.print(space);
  carin();
  carout();
  sen();

}
