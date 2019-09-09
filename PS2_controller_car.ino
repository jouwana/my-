/*
	robot controlled using a playstation 2 controller with two
	control modes, pad or stick.
	will stop if it sees an obstacle.
	ultrasonic (servo) can be moved using right stick.
	
	controller types limited.
*/


#include <PS2X_lib.h>  
#include "Ultrasonic.h"
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

#define PS2_DAT        13  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17
#define R A1
#define G A2
#define B A3
#define Buzz 8
#define IN1_PIN 2
#define IN2_PIN 3
#define IN3_PIN 4
#define IN4_PIN 5

#define pressures   false
#define rumble      false


int error = 0;
byte type = 0;
byte vibrate = 0;
bool on, offc;
int mode;
float perc;
int ly,lx,ry,rx; // PSS numbers
int t1,t2,t3,t4; // timers

void (* resetFunc) (void) = 0;

Ultrasonic ultrasonic (7,6); // trg echo
LiquidCrystal_I2C lcd(0x27,16,2); //could be 3F
PS2X ps2x;

Servo myservo;

void setup()
{
  on = false;
  offc = false;
  mode = 0;
  myservo.attach(9);
  myservo.write(90);
  
  //setup pinmodes
  pinMode(IN1_PIN,OUTPUT);
  pinMode(IN2_PIN,OUTPUT);
  pinMode(IN3_PIN,OUTPUT); 
  pinMode(IN4_PIN,OUTPUT);
  pinMode(Buzz, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  
  //initialize robot and connect controller
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("setting up");
  Serial.begin(115200);
  Serial.println("setting up");
  delay(500);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  //check for error
  if(error == 0)
  {
      Serial.println("Found Controller, configured successful ");
      Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
  }
  else if(error == 1)
    Serial.println("No controller found, check wiring.");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. ");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  // check controller type from known versions.
  type = ps2x.readType(); 
  switch(type)
  {
    case 0:
      Serial.println("Unknown Controller type found ");
      Serial.println("not supported, please use another controller");
      break;
    case 1:
      Serial.println("DualShock Controller found ");
      break;
    case 2:
      Serial.println("GuitarHero Controller found ");
      Serial.println("not supported, please use another controller");
      break;
    case 3:
      Serial.println("Wireless Sony DualShock Controller found ");
      break;
   }
   
   
   delay(500);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("click start to");
   lcd.setCursor(0,1);
   lcd.print("turn on");
}

//driving funtions:

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
  delay(50);
}

void left(){
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(R,255);
  analogWrite(G,255);
  analogWrite(B,0);
  delay(50);
}


void right() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
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

//-------------------------------------------------

// move based on ch value
void Move(char ch)
{
  switch(ch)
  {
  case 'r':
    right();
    break;
  case 'l':
    left();
    break;
  case 's':
    STOP();
    break;
  case 'f':
    forward();
    break;
  case 'b':
    backward();
    break;
  }
}


int ReadDistance()
{
  int dist;
  dist = ultrasonic.read(CM);
  return dist;
}

void OnOff() // checks if we're turning the controller on or off
{
  // if the start button is pressed
  if(ps2x.ButtonPressed(PSB_START))
  {
	// check if the controller is turned on
    switch(on)
    {
		
      case false: // turn on controller
        on = true;
        Serial.println("controller turned on");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print ("turned on");
        delay(500);
        offc = false;
        break;
		
		
      case true: // setup turn off mode
        offc = true;
        t1=millis()/100;
        Serial.println("hold button for 2 seconds for turn off");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("hold start to");
        lcd.setCursor(0,1);
        lcd.print("turn off");
        break;
    }
  }
  //while button is pressed, if it remains pressed for 2
  //seconds turn off, otherwise continue along
  if(offc && ps2x.Button(PSB_START))
  {
    t2 = millis()/100;
    if(t2-t1 >= 20)
    {
      on = false;
      Serial.println("controller turned off");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print ("turned off");
      delay(500);
      lcd.setCursor(0,1);
      lcd.print("wait for start");
      mode = 0;
      offc = false;
    }
  }
  else if (on && offc)
  {
    Serial.println("not turning off, continue previous action");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("no turn off");
    lcd.setCursor(0,1);
    lcd.print("continue");
    delay(500);
    lcd.clear();
    offc = false;
  }
}

// select control mode, pad or stick
void SelectMode()
{
  bool selected = false;
  while(!selected)
  {
    ps2x.read_gamepad();
    OnOff();
    if(!on) return;
    if (ps2x.ButtonPressed(PSB_L1))
    {
      mode = 1;
      Serial.println("PAD selected");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PAD selected");
      selected = true;
    }
    else if (ps2x.ButtonPressed(PSB_R1))
    {
      mode = 2;
      Serial.println("STICK selected");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("STICK selected");
      selected = true;
    }
    
  }
  t3 = 0;
  return;
}


void buzz(int i)
{
  digitalWrite(Buzz,i);
}


void loop()
{
  char ch;
  int dist;
  if(error == 1 || type == 2 || type == 0) //skip loop if no controller found
    resetFunc();
  ps2x.read_gamepad(false, vibrate);
  
  // check if on or off.
  OnOff();
  
  
  // if on and not trying to turn off then:
  if(on && !offc)
  {
    if(ps2x.ButtonPressed(PSB_L2))//turn on buzzer
      buzz(1);
      
    if(ps2x.ButtonPressed(PSB_R2))//turn off buzzer
      buzz(0);
      
	// change control mode
    if(ps2x.ButtonPressed(PSB_SELECT))
      mode = 0;
      
    if(mode == 0)
    {
      Serial.println("please select wheel operation mode: L1-PAD , R2-STICK");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("select mode:");
      lcd.setCursor(0,10);
      lcd.print("L1-PAD,R1-STICK");
      SelectMode();
    }
	
	
    t4 = millis()/100;
    if(t4-t3 >= 3)
    {
      dist = ReadDistance();
      Serial.print(dist);
      Serial.println("CM");
      if(mode == 1) //control using pads
      {
        if(ps2x.Button(PSB_PAD_DOWN) || ps2x.Button(PSB_CROSS))
        {
          Serial.println("DOWN ON");
          ch = 'b';
        }
      
        else if(ps2x.Button(PSB_PAD_UP) || ps2x.Button(PSB_TRIANGLE)) 
        {      //will be TRUE as long as button is pressed
          Serial.println("Up ON");
          ch = 'f';
        }
        else if(ps2x.Button(PSB_PAD_RIGHT) || ps2x.Button(PSB_CIRCLE))
        {
          Serial.println("Right ON");
          ch = 'r';
        }
        else if(ps2x.Button(PSB_PAD_LEFT) || ps2x.Button(PSB_SQUARE))
        {
          Serial.println("LEFT ON");
          ch = 'l';
        }
        else 
        {
          Serial.println("no movement");
          ch = 's';          
        }
        if (dist <= 25 && ch != 's' && ch != 'b')
        {
          Move('s');
          delay(250);
          Serial.println("OBSTACLE AHEAD!");
          lcd.setCursor(0,1);
          lcd.print("OBSTACLE AHEAD!");         
        }
        else
        {
          lcd.setCursor(0,1);
          lcd.print("                ");
          Move(ch);
        }
      }
	  
	  
	  
      if(mode == 2) // control using sticks
      {
        ly = ps2x.Analog(PSS_LY); //Left stick, Y axis. Other options: LX, RY, RX 
        Serial.print(ly); 
        Serial.print(",");
        lx = ps2x.Analog(PSS_LX);
        Serial.print(lx);  
        Serial.print(",");
        lcd.setCursor(0,1);
        lcd.print("                ");
          if (ly == lx == 255) //controller problems
          {
            ch = 's';
          }
          if((ly == 127 || ly == 128) && lx == 128)
          {
            Serial.println("standing still");
            ch = 's';
          }
        
          else if(abs(ly-127) > abs(lx-128))
          {
            if (ly < 127)
            {
              Serial.println("going forward");
              ch = 'f';
            }
            else
            {
              Serial.println("going down");
              ch = 'b';
            }
          }
          else if (lx > 127)
          {
            Serial.println("going right");
            ch = 'r';
          }
          else
          {
            Serial.println("going left");
            ch = 'l';
          }
        if (dist <= 25 && ch != 's' && ch != 'b')
        {
          Move('s');
          delay(250);
          Serial.println("OBSTACLE AHEAD!");
          lcd.setCursor(0,1);
          lcd.print("OBSTACLE AHEAD!");         
        }
        else
        {
          lcd.setCursor(0,1);
          lcd.print("                ");
          Move(ch);
        }
      }
	  
	  
	  // move ultrasonic (servo) based on right stick movement
	  // works for both control modes
      if(mode == 1 || mode == 2) 
      {
        rx = ps2x.Analog(PSS_RX); 
        Serial.println(rx);
        perc = rx/255.0;
        rx = 180 - (perc * 180);
        Serial.print("servo position ");
        Serial.println(rx);
        myservo.write(rx);
      }
      t3 = millis()/100;
    }  
  }  
}
