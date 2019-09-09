/*

	rfid locked door. recognized two already setup cards
	has a timelimit for unlocked opened and unooened door
	will sound an alarm if this timelimit is broken
	
*/

#include <Wire.h> 
#include <LiquidCrystal.h>
#include <DS3231_Simple.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LOCK_PIN 8
#define BUZZ_PIN A3
#define HALL_PIN A0

#define workerNmr 2
#define idNmr 4

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//DS3231_Simple Clock;
DateTime Time;

byte IDcode[] = {162,14,3,28,
                 183,246,36,217}; //This is the stored UID
byte noMatchNmr,worker;
String oldMin ="";
bool match,doorOpen;
//==============================================================================
void Buzzer(byte repeat,int delay1){
  for(byte i=0;i<repeat;i++){
  digitalWrite(BUZZ_PIN, HIGH);   // turn the Buzzer  on
  delay(delay1);              
  digitalWrite(BUZZ_PIN, LOW);    // turn the Buzzer off 
  delay(delay1);
  }    
}
//=======================================================================================
void Alarm(){
  while(1){ 
  digitalWrite(BUZZ_PIN, HIGH);   // turn the Buzzer  on
  delay(100);              
  digitalWrite(BUZZ_PIN, LOW);    // turn the Buzzer off 
  delay(100);
  digitalWrite(BUZZ_PIN, HIGH);   // turn the Buzzer  on
  delay(100);              
  digitalWrite(BUZZ_PIN, LOW);    // turn the Buzzer off 
  delay(500);
  if(digitalRead(HALL_PIN)==HIGH) return;
  }    
}
//======================================================================================
void setup() {
  Serial.begin(9600);
  DDRC=B00111110; //port C analog
  
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW); 
  
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  lcd.begin(16, 2);
  Clock.begin();
  Buzzer(1,1000);

}
//=======================================================================================  

void ReadClock(){ //read clock and write time to LCD
 String str;
  Time = Clock.read();
  if(oldMin==Time.Minute) return;
  oldMin=Time.Minute;
  // lcd.clear();
  str=(String)Time.Day+'/'+(String)Time.Month+'/'+'2'+'0'+(String)(Time.Year);
  //Serial.println(str);
  lcd.setCursor(0,0);
  lcd.print(str); 
  str=(String)Time.Hour+':'+(String)Time.Minute;//+':';+(String)Time.Second;
 
  //Serial.println(str);
  lcd.setCursor(11, 0);
  lcd.print(str); 
}

//=====================================================

void PrintWorkerName(){
   String worker1="Admin";
   String worker2="user";
   String noName="No Name";
   String str;
   
  switch(worker) {
   case 1:str=worker1 ;break;
   case 2:str=worker2 ;break;
   default :str=noName;
  }
  
 Serial.println("\nI know this card!");
 Serial.println("Welcome ");Serial.print(str);
 Serial.println("Open the Door ");
  lcd.clear();
  lcd.print("Welcome ");lcd.print(str);
  lcd.setCursor(0, 1);
  lcd.print("Open the Door "); 
}
//=======================================================

void UnlockDoor(){ 
 int cnt = 0;
  doorOpen=0;
PrintWorkerName();
digitalWrite(LOCK_PIN, HIGH); //turn light on
while(1){
  while(digitalRead(HALL_PIN)==HIGH){
    delay(500);
    cnt++;
    Serial.println(cnt);    
    if(cnt>=10){  // if door didnt open after 5 seconds, then lock it
      digitalWrite(LOCK_PIN, LOW); //  turn light off
      ReadClock();
      lcd.setCursor(0,1);
      lcd.print("Door is closed");
      Buzzer(3,100); 
      delay(700);
      return;  
     }
   }
cnt = 0; 
  while(digitalRead(HALL_PIN)==LOW){ //if door remains open for more than 5 seconds, then turn on alarm
    digitalWrite(LOCK_PIN, LOW); //  turn light off
    lcd.clear();
    ReadClock();
    lcd.setCursor(0,1);
    lcd.print("Door is open");
    delay(500);
    cnt++;
    Serial.println(cnt);
    if(cnt>=10)
    {
    Alarm();
    return;
    } 
   }
   
  }
}
//========================================================

void ReadRFID(){
  rfid.PICC_ReadCardSerial();
   Buzzer(1,500);
  Serial.println("The NUID tag is: ");
  for (byte i = 0; i < 4; i++) {
     Serial.print(rfid.uid.uidByte[i]);
     Serial.print(" ");
    }
}

//========================================================

void CheckID(){
  
  for (byte j = 0; j < workerNmr; j++) { 
    match=1; 
    for (byte i = 0; i < idNmr; i++) {

          if((rfid.uid.uidByte[i] != IDcode[i+j*4])){ 
        match=0;
        break;
       }     
    }   ////end for (byte i....
    if(match){
      worker=j+1;
      Serial.println(worker);
      return; 
    }
  }   //end for (byte j....
}
//=================================================================

void loop() {
 
 oldMin="00";
 lcd.clear(); 
 lcd.setCursor(3,0);
 lcd.print("Greetings!");
 lcd.setCursor(1,1);
 lcd.print("Enter The Tag"); 
 while(1){
  ReadClock();
  if(  rfid.PICC_IsNewCardPresent()){
    ReadRFID();
    CheckID();
    if(match){
      noMatchNmr=0;     
      UnlockDoor();
      break; 
    }
     else{
      noMatchNmr++;
      if(noMatchNmr>=2)
      Buzzer(3,1000);//block
    }
  }
}
}
//===========================================================
