/*
	temperature and bpm measuting robot controlled
	with BT, sends information back to be displayed 
	on the app.
*/


/*Symbol list:
 * RECIEVE from app:
 * 'K' -> mesure bpm
 * 'D' -> mesure temperature
 * 
 * SEND to app:
 * 'A' -> send sensor1 temperature to textbox
 * 'W' -> send sensor2 bpm to textbox 
 * 'a' -> send color to sensor1 lightbulb
 * 'L' -> send color to sensor2 lightbulb
 */




#include<LiquidCrystal.h>
#include <SoftwareSerial.h>
#define SensorPin 8 // sensor pin
#define RLed 9  //-> green lightbulb on transmitter
#define GLed 10  //-> red lightbulb on transmitter
#define buzzer 13
#define MaxTmpr1  39
#define MinTmpr1 32
LiquidCrystal lcd(2,3,4,5,6,7);
SoftwareSerial BT(11, 12); // RX, TX

int c,bpm,i;
char ch;
String str;   
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
  
  for (i=9; i<=10;i++)
  {
    pinMode(i,OUTPUT);
    digitalWrite(i,0);
  }
  
  pinMode(A0,INPUT);
  
  //start LCD
  lcd.begin(16,2);
  BT.begin(9600);
  Serial.begin(9600);
  Serial.println("hello");
  
  //create custom characters for "heart" sign
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
  lcd.print("tunring on");
  lcd.setCursor(0,1);
  lcd.print("Hello!");
  delay (3000);
  
}

//Send the rgb of char "color" to bulb "sentTo"
void SendColor(char color, char sentTo) 
{
  switch (color){
    case 'r'://red
    str="*"+String(sentTo)+"R255G0B0*";
    break;
    case 'g'://green
    str="*"+String(sentTo)+"R0G255B0*";
    break;
    case 'o'://orange
    str="*"+String(sentTo)+"R255G126B0*";
    break;  
  }
  BT.print(str); 
}

//Send the string "str" to textbox "sentTo"
void SendString (String str, char sentTo)
{
      str="*"+String(sentTo)+str+"*";
      BT.print(str); 
}

// measure bpm
void mesurebpm()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Put your finger");
  lcd.setCursor(0,1);
  lcd.print("on the sensor");
  delay (3000);
  lcd.clear();
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("Please wait...");
  delay(1000);
  
  c=0; //beats per 5 seconds
  bpm=0;
  t1=millis()/1000;
  while(1)
  { //read pulse for 5 seconds and light yellow bulb when it gets a pulse
        while(!digitalRead (SensorPin)) delay(1);
        {
           //print heart on lcd
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
    SendColor('r','L');
    digitalWrite(RLed,1);
    digitalWrite(GLed,0);
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
    SendColor('r','L');
    digitalWrite(RLed,1);
    digitalWrite(GLed,0);
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
    SendColor('g','L');
    digitalWrite(RLed,0);
    digitalWrite(GLed,1);
    lcd.setCursor(0,1);
    lcd.print("     Normal");
    delay(2250);
  }
  str="*W"+String(bpm)+"*";
  BT.print(str);
  Serial.println(str);
  
}

//measure temperature
void mesuretmp()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Put your finger");
  lcd.setCursor(0,1);
  lcd.print("on the sensor");
  delay(3000);
  lcd.clear();
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("Please wait...");
  delay(10000);
  float degC1;
  int adu1 = analogRead(A0);
  
  // Calculate the temperature.
  degC1 = adu1 * 0.488;
   // Turn it into a string.
  
  str=String(degC1,1)+ " C";

  // Display the header and the string.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sen1 = " + str + ".");
  Serial.println(str);
  SendString(String(degC1,1), 'A');
  if(degC1>MaxTmpr1)
  {
    SendColor('r','a');
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else if(degC1<MinTmpr1)
  {
    SendColor('o','a');
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else SendColor('g','a');
}

void loop() {
  // the main code:

  while(BT.available())
   {
     ch = BT.read();
     Serial.print("ch is: ");
     Serial.println(ch);
     if(ch == 'K')
     {
       mesurebpm();
       delay(4000);
     }
     break;
   }
   mesuretmp();
   delay(3000);

}
