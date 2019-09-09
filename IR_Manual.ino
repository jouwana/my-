/*
	IR controlled car with an option for automatic
	movement based on the distance from the fron of
	the car, with the preferance of moving forward
	of towards the left.
*/

#include "Ultrasonic.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

// set pins
#define IN1_PIN 2
#define IN2_PIN 3
#define IN3_PIN 4
#define IN4_PIN 5
#define Buzzer 11
#define IR 10
#define R 7
#define G 8
#define B 9

Ultrasonic ultrasonic (13,12); // trg echo
LiquidCrystal_I2C lcd(0x27,16,2); //could be 3F

int dist,t1=0,t2=0;
bool flag = true, start = true;
IRrecv irrecv(IR);
decode_results results;
unsigned long key_value = 0;

void setup (){
	
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  
  //set up pins
  pinMode(IN1_PIN,OUTPUT);
  pinMode(IN2_PIN,OUTPUT);
  pinMode(IN3_PIN,OUTPUT); 
  pinMode(IN4_PIN,OUTPUT);
  pinMode(Buzzer,OUTPUT);
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);
  
  irrecv.enableIRIn();
  
  Serial.println("Robot initialize");
  lcd.setCursor(0,0);
  lcd.print("Robot initialize");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("distance:   CM");
  
}

void buzz(){
  for (int j = 0; j < 2; j++){
  digitalWrite(Buzzer, HIGH);   
  delay (300);
  digitalWrite(Buzzer, LOW);   
  delay (300);
  }
}

// -----------------------------------------------------------
// driving function that also change the color of a lightbulb
// based on which direction it is going.

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
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(R,255);
  analogWrite(G,0);
  analogWrite(B,0);
  buzz();
  delay(50);
}

void left(){
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(R,255);
  analogWrite(G,255);
  analogWrite(B,255);
  delay(50);
}


void right() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
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

//---------------------------------------------------------

// automatic driving function
void Auto(int dist)
{
  if(dist >=25)
  {
    forward();
  }   
  else
  {
      STOP();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Obstacle ahead");
      lcd.setCursor(0,1);
      lcd.print("changing route");
      backward();
      delay(100);
      left();
      delay(500);
      flag = false;
  }

}

//----------------------------------------------------------

void loop (){ 
  // the timing is to prevent incrorrect clicks from IR
  // and to be able to fully read the distance on the lcd monitor.
  if (start || (t2-t1 >= 5))
  {
    dist = ultrasonic.read(CM);
    t1 = millis()/100;
    
    if(!flag)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("distance:   CM");
    }
    else 
    {
      lcd.setCursor(9,0);
      lcd.print("   ");
    }
    
    lcd.setCursor(9,0);
    lcd.print(dist);
    start = false;
  }
  
  else
  {
    t2 = millis()/100;
  }
  

  if (irrecv.decode(&results))
  {
	// to prevent incorrectly decoded clicks from IR
    if(!(results.value == 0xFFFFFFFF || results.value >= 0xFFFFFF || results.value == 0xFF))
        key_value = results.value;
	// write what was decoded as well as what is currenctly saved
	// as a way to make sure the information is correct
    Serial.print(results.value , HEX);
    Serial.print("  key:  ");
    Serial.println( key_value , HEX);
    irrecv.resume();
  }
  
  // choose operation based on IR key.
  switch (key_value)
  {
    case 0xFF18E7: //forward
    forward();
    break;
    case 0xFF5AA5: //right
    right();
    break;
    case 0xFF4AB5: //backward
    backward();
    break;
    case 0xFF10EF: //left
    left();
    break;
    case 0xFF38C7: //stop (ok)
    STOP();
    break;
    case 0xFF6897: //auto (*)
    Auto(dist);
    break;
  }

 
}
