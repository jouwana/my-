/*
	heart beat sensor that buzzes with high or low 
	blood pressure. adds a small heart beat on lcd
	while measuring beats.
*/


#include<LiquidCrystal.h>
#define SensorPin 8 // sensor pin
#define GLed 11  //-> green lightbulb on transmitter
#define RLed 12  //-> red lightbulb on transmitter
#define TRLed 9  //-> green lightbulb on receiver
#define TGLed 10 //-> red lightbulb on receiver
#define HBLed 13 //->  yellow lightbulb
#define buzzer A0

LiquidCrystal lcd(2,3,4,5,6,7);

int c,bpm,i;   
unsigned long t1,t2;

//set arrays for custom character
byte customChar1[8] = {0b00000,0b00000,0b00011,0b00111,0b01111,0b01111,0b01111,0b01111};
byte customChar2[8] = {0b00000,0b11000,0b11100,0b11110,0b11111,0b11111,0b11111,0b11111};
byte customChar3[8] = {0b00000,0b00011,0b00111,0b01111,0b11111,0b11111,0b11111,0b11111};
byte customChar4[8] = {0b00000,0b10000,0b11000,0b11100,0b11110,0b11110,0b11110,0b11110};
byte customChar5[8] = {0b00111,0b00011,0b00001,0b00000,0b00000,0b00000,0b00000,0b00000};
byte customChar6[8] = {0b11111,0b11111,0b11111,0b11111,0b01111,0b00111,0b00011,0b00001};
byte customChar7[8] = {0b11111,0b11111,0b11111,0b11111,0b11110,0b11100,0b11000,0b10000};
byte customChar8[8] = {0b11100,0b11000,0b10000,0b00000,0b00000,0b00000,0b00000,0b00000};


void setup() 
{
  
  // setup code, PinModes
  pinMode(SensorPin,INPUT);
  
  for (i=9; i<14;i++)
  {
    pinMode(i,OUTPUT);
    digitalWrite(i,0);
  }
  
  pinMode(A0,OUTPUT);
  //start LCD
  lcd.begin(16,2);
  
  //create custom characters for "heart" sign on LCD
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);
  lcd.createChar(4, customChar4);
  lcd.createChar(5, customChar5);
  lcd.createChar(6, customChar6);
  lcd.createChar(7, customChar7);
  lcd.createChar(8, customChar8);

  //start program
  lcd.setCursor(0,0);
  lcd.print("Turned on");
  lcd.setCursor(0,1);
  lcd.print("Hello!");
  delay (3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Put your finger");
  lcd.setCursor(0,1);
  lcd.print("on the sensor");
  delay (3000);
  
}



void loop() {
  // the main code:
  lcd.clear();
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("Please wait...");
  
  c=0; //beats per 5 seconds
  bpm=0;
  t1=millis()/1000;
  while(1)
  { //read pulse for 5 seconds and light yellow bulb when it gets a pulse
        while(!digitalRead (SensorPin)) delay(1);
        digitalWrite(HBLed,1);
        {
           //print heart sign on lcd
           lcd.setCursor(12, 0);
           lcd.write(byte(1));
           lcd.setCursor(13, 0);
           lcd.write(byte(2));
           lcd.setCursor(14, 0);
           lcd.write(byte(3));
           lcd.setCursor(15, 0);
           lcd.write(byte(4));
  
           lcd.setCursor(12, 1);
           lcd.write(byte(5));
           lcd.setCursor(13, 1);
           lcd.write(byte(6));
           lcd.setCursor(14, 1);
           lcd.write(byte(7));
           lcd.setCursor(15, 1);
           lcd.write(byte(8));
           c++;
        }
        while(digitalRead (SensorPin)) delay(1);
        {
           digitalWrite(HBLed,0);
           lcd.clear();
           lcd.setCursor(0,1);
           lcd.print("Please wait..");
        }
    t2=millis()/1000;

    if((t2-t1)>5) break;
    
  }

  bpm=12*c; //find bpm and print it
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    BPM = ");
  lcd.print(bpm);

  
  if(bpm>150) //if bpm is high, light red bulb
  {
    digitalWrite(RLed,1);
    digitalWrite(TRLed,1);
    digitalWrite(GLed,0);
    digitalWrite(TGLed,0);
    lcd.setCursor(0,1);
    lcd.print("High heartbeat!");
    for(i = 0; i< 8; i++)
    {
      if(i%2==0)
      {
        digitalWrite(buzzer,HIGH);
        delay (250);
      }
      
      else
      {
        digitalWrite(buzzer,LOW);
        if (i == 3) delay (500);
        else delay (250);
      }
    }
  }



  else if(bpm<60) //if bpm is low, light red bulb
  {
    digitalWrite(RLed,1);
    digitalWrite(TRLed,1);
    digitalWrite(GLed,0);
    digitalWrite(TGLed,0);
    lcd.setCursor(0,1);
    lcd.print( "Low heartbeat!");
    for(i = 0; i< 4; i++)
    {
      if(i%2==0)
        {
          digitalWrite(buzzer,HIGH);
          delay (250);
        }
        
      else
        {
          digitalWrite(buzzer,LOW);
          delay (500);
        }
    }
    delay(250); 
  }
  else //if bpm is normal light green bulb
  {
    digitalWrite(RLed,0);
    digitalWrite(TRLed,0);
    digitalWrite(GLed,1);
    digitalWrite(TGLed,1);
    lcd.setCursor(0,1);
    lcd.print("     Normal");
    delay(2250);
  }
  
}


