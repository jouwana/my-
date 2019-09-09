/*
	temperature sensing robot that sends the info back to
	an app using bluetooth, connected to a fan and a light 
	the will operate or change color based on temperatue.
	the app also has an indicator for the fan and the light.
	temperature will also be displayed on LCD
*/



/*Symbol list:
 * RECIEVE from app:
 * 'A' -> sensor one ON
 * 'S' -> sensor one OFF
 * 'B' -> sensor two ON
 * 'l' -> sensor two OFF 
 * repeat send every 50ms.
 * repeat send whilst switch is on.
 * 
 * SEND to app:
 * 'A' -> send sensor1 temperature to textbox and gauge
 * 'B' -> send sensor2 temperature to textbox and gauge
 * 'a' -> send color to sensor1 lightbulb (g - good, o - medium, r - high)
 * 'b' -> send color to sensor2 lightbulb (g - good, o - medium, r - high)
 * 'F' -> send color to Fan1 (g - ON, w - OFF)
 * 'f' -> send color to Fan2 (g - ON, w - OFF)
 * 
 * Gauge settings:
 * min text = 0
 * max text = 60
 * min value = 0
 * max value = 60
 * major devisions = 6
 * sub devisions = 5
 * current value = 0
 */
 
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define SENS1_PIN  A0
#define SENS1_PIN  A1 
#define Buzz_PIN  8
#define Fan1_PIN  9
#define Fan2_PIN  10
#define MaxTmpr1  31
#define MaxTmpr2  29
#define MedTmpr1 26
#define MedTmpr2 24

String str="";
int Sen1On = 0; // 1 if senor1 is on, 0 if its off
int Sen2On = 0; // 1 if senor2 is on, 0 if its off
// Set the LCD address to 0x3f for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);
// Set the BT address
SoftwareSerial BT(11, 12); // RX, TX

void setup()
{
  lcd.init();                      
  lcd.backlight();
  lcd.print("Hello, Romio!");
  Serial.begin(9600);
  BT.begin(9600);
  pinMode(Buzz_PIN, OUTPUT);
  pinMode(Fan1_PIN, OUTPUT);
  pinMode(Fan2_PIN, OUTPUT);
  digitalWrite(Buzz_PIN, HIGH);
  delay(1000);
  digitalWrite(Buzz_PIN, LOW);

}


void SendColor(char color, char sentTo) //Send the rgb of char "color" to bulb "sentTo"
{
  switch (color){
    case 'w'://white
    str="*"+String(sentTo)+"R255G255B255*";
    break;
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

void SendString (String str, char sentTo)//Send the string "str" to textbox "sentTo"
{
      str="*"+String(sentTo)+str+"*";
      BT.print(str); 
}


void loop()
{
  String str;
  float degC1,degC2,ch;

//LCD=====================  
      // Read the LM35 sensor1 on A0. 
 
      int adu1 = analogRead(A0);
       
      // Calculate the temperature.
      degC1 = adu1 * 0.488;
    
      // Turn it into a string.
      
      str=String(degC1,1)+ " C";

      // Display the header and the string.
      lcd.setCursor(0,0);
      lcd.print("Sen1 = " + str + ".");
      Serial.println(str);
      // Read the LM35 sensor2 on A1.
  
      int adu2 = analogRead(A1);
    
      // Calculate the temperature.
      degC2 = adu2 * 0.488;
    
    
      // Turn it into a string.
      str=String(degC2,1)+ " C";
    
      // Display the header and the string.
      lcd.setCursor(0,1);
      lcd.print("Sen2 = " + str + ".");
      Serial.println(str);


//BT=====================
  while(BT.available())
   {
     ch = BT.read();
     if(ch == 'A') //sensor 1 is ON
      {
      // Display the temperature of sensor1 on the App.
     
      SendString(String(degC1,1), 'A');
      Sen1On = 1;
   }
   else if(ch == 'B') //sensor 2 is ON
     {
      // Display the temperature of sensor2 on the App.
      SendString(String(degC2,1), 'B');
      Sen2On = 1;
  }
  else if(ch == 'S') //sensor 1 is OFF
      {
      // Stop temperature sensor1 on the App, and reset it to 0.
      SendString("0", 'A');
      SendColor('w','a');
      Sen1On = 0;
      }
  else if(ch == 'l') //sensor 2 is OFF
      {
      // Stop temperature sensor2 on the App, and reset it to 0.
     
      SendString("0", 'B');
      SendColor('w','b'); 
      Sen2On = 0;
      }

	}

	//Fan And Buzzer Work------------
	if(degC1>MaxTmpr1) {
		 digitalWrite(Fan1_PIN, LOW); 
		 digitalWrite(Buzz_PIN, HIGH);
		 delay(250);
		 digitalWrite(Buzz_PIN, LOW);
		 delay(100); 
		 SendColor('r','a');
		 SendColor('g','F');
			 
		}
	else if(degC1>MedTmpr1){
	 digitalWrite(Fan1_PIN, HIGH);
	 SendColor('o','a');
	 SendColor('w','F');   
	}
	else if(Sen1On == 1){
	 digitalWrite(Fan1_PIN, HIGH); 
	 SendColor('g','a');
	 SendColor('w','F');
	}
	else {
	digitalWrite(Fan1_PIN, HIGH);
	}

   if(degC2>MaxTmpr2) {
	 digitalWrite(Fan2_PIN, LOW); 
	 digitalWrite(Buzz_PIN, HIGH);
	 delay(250);
	 digitalWrite(Buzz_PIN, LOW);
	 delay(100);    
	 SendColor('r','b');
	 SendColor('g','f');
	}
	else if(degC2>MedTmpr2){
	 digitalWrite(Fan2_PIN, HIGH);
	 SendColor('o','b');
	 SendColor('w','f'); 
	}
	else if(Sen2On == 1){
	 digitalWrite(Fan2_PIN, HIGH);
	 SendColor('g','b');
	 SendColor('w','f');
	}
	else {
	digitalWrite(Fan2_PIN, HIGH);
	} 
  delay(1000);
}


   
