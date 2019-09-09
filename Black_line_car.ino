/*
	self moving car that follows a black line on the floor
	that also stops completely if there is an obsatcle ahead
*/


#include <SoftwareSerial.h>
#include "Ultrasonic.h"
#include <LiquidCrystal_I2C.h>

#define IN1_PIN 2
#define IN2_PIN 3
#define IN3_PIN 4
#define IN4_PIN 5
#define EN1_PIN 11
#define EN2_PIN 10
#define R 6
#define G A2
#define B 1

#define Buzz 13

//SENSOR PINS
#define S1 9 
#define S2 8
#define S3 7  
 
int Sensor1 = 0;  // right sensor
int Sensor2 = 0;  // centre sensor
int Sensor3 = 0;  // left sensor

LiquidCrystal_I2C lcd(0x27,16,2);   //address can be 0x3F

Ultrasonic ultrasonic(12, A1);

int distance;


void setup() 
{
  Serial.begin(9600);
  lcd.init();                       
  lcd.backlight();
  
  //setup penmodes
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(EN1_PIN, OUTPUT);
  pinMode(EN2_PIN, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(Buzz,OUTPUT);
  for (int i=7;i<=9;i++)
    pinMode(i,INPUT);
}



//print the distance
void PrintDistance(int d)
{
  lcd.setCursor(0,0);
  lcd.print("distance:");
  lcd.print("     ");
  lcd.setCursor(9,0);
  lcd.print(distance);
  lcd.print("CM");
}

//Robot movement functions start:
void RobotForward()
{
  lcd.setCursor(0,1);
  lcd.print("going forward  ");
  analogWrite(EN1_PIN, 180);
  analogWrite(EN2_PIN, 180);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  digitalWrite(R, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(B, LOW);
}


void RobotRight()
{
  lcd.setCursor(0,1);
  lcd.print("going right     ");
  analogWrite(EN1_PIN, 150);
  analogWrite(EN2_PIN, 150);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  digitalWrite(R, HIGH);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);

}

void RobotLeft()
{
  lcd.setCursor(0,1);
  lcd.print("going left      ");
  analogWrite(EN1_PIN, 150);
  analogWrite(EN2_PIN, 150);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, HIGH);

}

void RobotStop()
{
  lcd.setCursor(0,1);
  lcd.print("Robot Stopping  ");
  analogWrite(EN1_PIN, LOW);
  analogWrite(EN2_PIN, LOW);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);

}
// Robot movement functions end.


void loop()
{
  // check and print distance 
  distance = ultrasonic.read();
  PrintDistance(distance); 
  Serial.print("distance is:");
  Serial.println(distance);
  
  
  if(distance <= 20)
  {
	//stop if there is an obstacle
    RobotStop();
    lcd.setCursor(0,1);
    lcd.print("OBSTACLE! STOP! ");
    digitalWrite(Buzz,1);
  }
  else
  {
	//read and print sensor values
    digitalWrite(Buzz,0);
    Sensor1 = digitalRead(S1);
    Sensor2 = digitalRead(S2);
    Sensor3 = digitalRead(S3);
    Serial.print(" s1:");
    Serial.print(Sensor1);
    Serial.print(" s2:");
    Serial.print(Sensor2);
    Serial.print(" s3:");
    Serial.println(Sensor3);
	
	//choose direction based on sensor readings
    if((Sensor1 && !Sensor2 && !Sensor3)||(Sensor1 && Sensor2 && !Sensor3)) 
      RobotLeft();      
    else if((Sensor3 && !Sensor1 && !Sensor2)||(Sensor3 && Sensor2 && !Sensor1))
      RobotRight();         
    else if((Sensor2 && !Sensor1 && !Sensor3)||(Sensor2 && Sensor1 && Sensor3))
      RobotForward();
    else
    {              
      RobotStop();
      lcd.setCursor(0,1);
      if(!Sensor2 && !Sensor1 && !Sensor3)
      lcd.print("ALL WHITE      ");
    } 
	// negate the value of the 'if' operation if you want
	// the robot to follow white line instead of black
  }
}
 
 
