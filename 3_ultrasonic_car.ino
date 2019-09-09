/*
	self moving car that uses 3 ultrasonics to gauge distance
	and choose direction.
*/


#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>

//define motor pins
#define LM_PWM 3    //  PWM for L_Motor  
#define RM_PWM 5    //  PWM for R_Motor
#define LM_DIR 2    //  direction for L_Motor  1-forward    0-backward
#define RM_DIR 4    //  direction for R_Motor  1-forward    0-backward

//define ultrasonic pins
#define L_ECHO A0   // left echo pin  
#define L_TRG 10    // left triggrt pin
#define M_ECHO A1   // mid echo pin
#define M_TRG 11    // mid triggrt pin
#define R_ECHO A2   // right echo pin
#define R_TRG 12    // right triggrt pin

#define Buzz 13     // buzzer pin

//define rgbLed pins
#define RLed 6     // red rgb pin
#define GLed 7     // green rgb pin
#define BLed 8     // blue rgb pin



Ultrasonic ultrasonic1(L_TRG,L_ECHO);  // left ultra
Ultrasonic ultrasonic2(M_TRG,M_ECHO);  // mid ultra
Ultrasonic ultrasonic3(R_TRG,R_ECHO);  // right ultra
LiquidCrystal_I2C lcd(0x3F,16,2);   //address can be 0x27

int i;
char dir;


// stop if speeds are 0, backwards  with negative speed, forwards with positive speeds
// LM_Speed is the speed of the left motor, RM_Speed for the right motor
void MotorsPWM(int LM_Speed,int RM_Speed) 
{
  if(LM_Speed>=0)
      digitalWrite(LM_DIR ,0);     
  else
    {
      LM_Speed+=255;
      digitalWrite(LM_DIR ,1);
    } 

  if(RM_Speed>=0)
      digitalWrite(RM_DIR ,0);  
  else
    {
      RM_Speed+=255;
      digitalWrite(RM_DIR ,1);
    }
      
  analogWrite(LM_PWM, LM_Speed);
  analogWrite(RM_PWM, RM_Speed);
}

//print the distances into LCD and Serial
void PrintDistance(int dist1, int dist2, int dist3)
{   
  Serial.print("F:");
  Serial.println(dist2);
  Serial.print("R:");
  Serial.println(dist3);
  Serial.print("L:");
  Serial.println(dist1);

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Front:");
  lcd.print(dist2);
  lcd.setCursor(1,1);
  lcd.print("R:");
  lcd.print(dist3);
  lcd.setCursor(9,1);
  lcd.print("L:");
  lcd.print(dist1); 
}

//choose the direction based on distnaces, preferance is F-L-R-B
char ChooseDirection()
{
  int dist1,dist2,dist3;
  dist1 = ultrasonic1.read(CM);
  dist2 = ultrasonic2.read(CM);  
  dist3 = ultrasonic3.read(CM);

  PrintDistance(dist1,dist2,dist3);
  
  if(dist2>20)
    return 'F';

  MotorsPWM(0,0);

  if(dist1>20)
    return 'L';

  if(dist3>20)
    return 'R';

  return 'B';
}

//go to the direcrion D, if direction is B, go back, buzz, then turn right
void EnactDirection(char D)
{
  ChooseColor(D);
  switch(D)
  {
    case 'F': MotorsPWM(150,150);
              break;
    case 'R': MotorsPWM(0,-150);
              break;
    case 'L': MotorsPWM(-150,0);
              break;
    case 'B': MotorsPWM(-125,-125);
              tone(Buzz, 1000);
              delay(2000);
              noTone(Buzz);
              MotorsPWM(0,-150);
              ChooseColor('R');
              Serial.println("going to: R");
              delay(2000);
              break;
  }
}

//choose color based on the direction
void ChooseColor(char D)
{
  switch(D)
  {
    case 'F': EnactColor(0,1,0);
              break;
    case 'R': EnactColor(0,0,1);
              break;
    case 'L': EnactColor(1,1,0);
              break;
    case 'B': EnactColor(1,0,0);
              break;
              
  }
}

//display the colors on the rgb led 
void EnactColor(int R, int G, int B)
{
  digitalWrite(RLed,R);
  digitalWrite(GLed,G);
  digitalWrite(BLed,B);
}

void setup()
{
  for(i=2;i<9;i++)
     pinMode(i,OUTPUT);
  pinMode(Buzz,OUTPUT);
  lcd.init();                       
  lcd.backlight();
  Serial.begin(9600);
}


void loop()
{
  dir = ChooseDirection();
  Serial.print("going to: ");
  Serial.println(dir);
  EnactDirection(dir);
  delay(1000);

}
