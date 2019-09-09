/*
	BT controlled car with an automatic option that chooses
	its direction based on an ultrasonic that moves in a 180
	degree angle, with a preference to go right after going 
	backwards upon meeting an obstacle.
*/



#include "Ultrasonic.h"
#include <Servo.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// set pins
const int IN1_PIN=2;
const int IN2_PIN=3;
const int IN3_PIN=4;
const int IN4_PIN=5;
const int Buzzer = 11;
const int R=6;
const int G=9;
const int B=10;

//global values
int dist=0;
int i=0;
int flag = 0; // 1 if car moved backwards, 0 if not
int A=0; 
int select;

int pos;

char ch,temp;

const char D = 223;



Servo myservo;
SoftwareSerial BT(7, 8); // RX, TX
Ultrasonic ultrasonic (12,13); // trg echo
LiquidCrystal_I2C lcd(0x27,16,2); //could be 3F instead of 27



void setup ()
{
  lcd.init();
  lcd.backlight();
  myservo.attach(A1);
  Serial.begin(9600);
  myservo.write(90);
  
  //set pinmode
  pinMode(IN1_PIN,OUTPUT);
  pinMode(IN2_PIN,OUTPUT);
  pinMode(IN3_PIN,OUTPUT); 
  pinMode(IN4_PIN,OUTPUT);
  pinMode(Buzzer,OUTPUT);
  
  BT.begin(9600);
  pos = 90;
  lcd.setCursor(0,0);
  lcd.print("Robot initialize");
  Serial.println("Robot initialize");
}

void buzz()
{
  for (int j = 0; j < 2; j++){
  digitalWrite(Buzzer, HIGH);   
  delay (300);
  digitalWrite(Buzzer, LOW);   
  delay (300);
  }
}

// driving functions:

void forward(){
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(R,0);
  analogWrite(G,255);
  analogWrite(B,0);
  delay(50);
}

void backward(){
  analogWrite(R,255);
  analogWrite(G,0);
  analogWrite(B,0);
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  buzz();
  delay(50);
}

void left(){
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(R,255);
  analogWrite(G,255);
  analogWrite(B,0);
  delay(50);
}


void right() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(R,0);
  analogWrite(G,0);
  analogWrite(B,255);
  delay(50);
}


void STOP(){
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  digitalWrite(R,0);
  digitalWrite(G,0);
  digitalWrite(B,0);
  delay(50);                    
}
//--------------------------------------

//automatic control
void automatic ()
{
  myservo.write(pos);
  Serial.print("position is:");
  Serial.println(pos);
  delay(250);
  dist = ultrasonic.read();

  //print distance
  lcd.setCursor(0,1);
  lcd.print("Dist=");
  lcd.setCursor(6,1);
  lcd.print("   ");
  lcd.setCursor(6,1);
  lcd.print(dist);
  lcd.setCursor(12,1);
  lcd.print("    ");
  lcd.setCursor(12,1);
  lcd.print(pos);
  lcd.print(D);
  Serial.print("distance = ");
  Serial.println(dist);
	
  //check distance and choose movement
  if(dist <= 20)
  {
    STOP();
    pos+=45;
    if(pos>=225)
    { 
      backward();
      pos=0;
    }
    delay(1000);
  }
  else
  {
    select = pos;
    pos=90;
    switch(select)
    {
      case 0: right(); delay(1000); break;
      case 45: right(); delay(1000); break;
      case 90: forward(); break;
      case 135: left(); delay(1000); break;
      case 180: left(); delay(1000); break;
    }
  }
  return;
}




void loop ()
{
  if(BT.available())
  {
    ch = BT.read();
    if(temp == 'A' and ch != 'S') //if automatic button is not turned off
    ch = temp;  // dont let manual controls affect movement
    else temp = " "; 
	
    if(ch!='A')
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual");
    }
    else
    {
      lcd.clear();
      pos = 90;
    }
  }

  //choose control / direction
  switch (ch)
  {
      case 'A': // automatic
      lcd.setCursor(0,0);
      lcd.print("Automatic");
      automatic();
      temp = ch;
      break;
      case 'F': //forward
      forward();
      break;
      case 'R': //right
      right();
      break;
      case 'B': //backward
      backward();
      break;
      case 'L': //left
      left(); 
      break;
      case 'S': //stop automatic
      STOP();
      break;
      case 's': //stop manual
      STOP();
      break;
  }
}
