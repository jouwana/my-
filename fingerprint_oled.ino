/*
	uses fingerprints to identifiy and welcome users.
	uses an OLED screen to write on.
	cannot be used to add unidentified users.
	will also play music when a known user is welcomed.
	
*/



#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "pitches.h"
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);
SoftwareSerial mySerial(2, 3);


int i; //stage number + ID number
#define GLED 7
#define RLED 8
#define Buzz 6
#define Relay 12


void setup() {
    i = -2;

    pinMode(GLED,OUTPUT);
    pinMode(RLED,OUTPUT);
    pinMode(Buzz,OUTPUT);
    pinMode(Relay,OUTPUT);
    
//Fingerprint sensor module setup
    Serial.begin(9600);
// set the data rate for the sensor serial port
    Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    }
    else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) {
            delay(1);
        }
    }
//OLED
    u8g.setColorIndex(1); // Instructs the display to draw with a pixel on. 
}

void loop() {

  if(i == -2)
  {
    u8g.firstPage();
    do {  
       displayMainScreen();
    } while( u8g.nextPage() );
  }
  int i = getFingerprintIDez();
  Serial.println(i);
  if( i != -3)
  {
    u8g.firstPage();
    do {  
       PrintName(i);
    } while( u8g.nextPage() );
    if(i == -1)
    {
      unauthorizedPrint();
      unauthorizedPrint();
    }
    else displayUserGreeting();
    i = -2;
  }
}



// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
    Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK && p == FINGERPRINT_NOFINGER)
      return -3;
    if (p != FINGERPRINT_OK) return -1;
    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return -1;
    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) return -1;
// found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID;
}

void displayMainScreen() {
  u8g.setFont(u8g_font_6x10);
  digitalWrite(RLED,HIGH);
  u8g.setPrintPos(30, 25);
  u8g.print( "Waiting for "); 
  u8g.setPrintPos(0, 35);
  u8g.print("authorized fingerprint");
}

void unauthorizedPrint()
{
  digitalWrite(RLED,HIGH);
  digitalWrite(Buzz,HIGH);
  delay(250);
  digitalWrite(RLED,LOW);
  digitalWrite(Buzz,LOW);
  delay(250);
}

void displayUserGreeting() {
  digitalWrite(GLED,HIGH);
  digitalWrite(Relay,HIGH);
  music();
  delay(2500);
  digitalWrite(GLED,LOW);
  digitalWrite(Relay,LOW);
}

void music() {
  int melody[] = {
      NOTE_C4,NOTE_G4,NOTE_G4,NOTE_A4,NOTE_G4,0,NOTE_B4,NOTE_C5,0
  };
  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4,4};
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 9; thisNote++) {
      //to calculate the note duration, take one second divided by the
      // note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(Buzz, melody[thisNote], noteDuration);
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
// stop the tone playing:
      noTone(Buzz);
    }
}

void PrintName(int i){
    u8g.setFont(u8g_font_osb21);
    if (i == -1)
    {
      u8g.setFont(u8g_font_unifont);
      u8g.setPrintPos(28,35);
      u8g.print("UNKNOWN!");
      return;
    }
    u8g.setPrintPos(0, 25);
    u8g.print( "Welcome ");
    digitalWrite(RLED,LOW);
    int fingerprintID = i;
    u8g.setPrintPos(0, 55);
    if(fingerprintID == 1) {
        u8g.print( "Admin" );
    }
    else if(fingerprintID == 6) {
        u8g.print( "Ruba" );
    }
    else if(fingerprintID == 7) {
        u8g.print( "Ameer" );

    }else u8g.print( "Guest" );
    
}
