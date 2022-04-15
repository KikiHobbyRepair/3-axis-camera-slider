//3 axis Camera Slider --- Kiki Hobby Repair
//Under development - USE IT AT YOUR OWN RISK!


#include <Arduino.h>
#include <Rotary.h>
#include <OneButton.h> 
#include <SpeedyStepper.h>
#include <U8g2lib.h>
#include <stdint.h>


Rotary r = Rotary(2, 3);
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  

#define enablePin       8                      //Motor enable pin
OneButton button(11,true);                     //Rotary Encoder Button
#define stepsPerMm      80                     //Slider step/mm (with 1.8deg motor)
#define stepPerDeg      17.77778               //rotate/tilt steps/degree (with 0.9deg motor)

const int MOTOR1_STEP_PIN = 9;
const int MOTOR1_DIRECTION_PIN = 10;
const int MOTOR2_STEP_PIN = 4;
const int MOTOR2_DIRECTION_PIN = 5;
const int MOTOR3_STEP_PIN = 6;
const int MOTOR3_DIRECTION_PIN = 7;

SpeedyStepper stepper1;
SpeedyStepper stepper2;
SpeedyStepper stepper3;

unsigned char result;
float encPos = 0;
int encMin = 0;
int encMax = 4;
int encSteps = 1;
int enter = 0;
int val = 0;
int slideDist = 0;
float rotAngle = 0;
float moveSpeed = 0;
float rotSpeed = 0;
float tiltSpeed = 0;
float tiltAngle = 0;



void setup() {
  
  u8g2.begin();
  u8g2.enableUTF8Print();
  pinMode(enablePin, OUTPUT);                        
  r.begin();
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
  
  button.attachClick(click);
  stepper1.connectToPins(MOTOR1_STEP_PIN, MOTOR1_DIRECTION_PIN);
  stepper2.connectToPins(MOTOR2_STEP_PIN, MOTOR2_DIRECTION_PIN);
  stepper3.connectToPins(MOTOR3_STEP_PIN, MOTOR3_DIRECTION_PIN);
  stepper1.setStepsPerMillimeter(80);
  stepper2.setStepsPerRevolution(6400);
  stepper3.setStepsPerRevolution(6400);
  stepper1.setAccelerationInMillimetersPerSecondPerSecond(50);
  stepper2.setAccelerationInMillimetersPerSecondPerSecond(50);
  stepper3.setAccelerationInMillimetersPerSecondPerSecond(50);
  
  
}

void loop() {
  enter = 0;
  encMin = 0;
  encMax = 4;
  encSteps = 1;
  digitalWrite(enablePin, LOW);
  button.tick();
  
   u8g2.firstPage();
      do {
        
        u8g2.setFont(u8g2_font_osb18_tr);
        
           
        if (encPos == 0) {
          u8g2.setCursor(40, 40);  
          u8g2.print("Slide");
          if (enter == 1)
            {                     
              slide();
            }
          }
        
        else if  (encPos == 1) {
          
          u8g2.setCursor(15, 30);
          u8g2.print("Slide and"); 
          u8g2.setCursor(30, 55);
          u8g2.print("Rotate"); 
          if (enter == 1)
            {                     
              slideAndRotate();
            }   
          }
          
           else if  (encPos == 2) {
           u8g2.setCursor(40, 20);
          u8g2.print("Slide"); 
          u8g2.setCursor(5, 40);
          u8g2.print("Rotate and"); 
          u8g2.setCursor(46, 64);
          u8g2.print("Tilt"); 
          if (enter == 1)
            {                     
              slideAndRotateAndTilt();
            }  
          
          } 
        
          else if  (encPos == 3) {
           u8g2.setCursor(15, 30);
          u8g2.print("Slide and"); 
          u8g2.setCursor(46, 55);
          u8g2.print("Tilt"); 
          if (enter == 1)
            {                     
              slideAndTilt();
            } 
          }
        
          else if  (encPos == 4) {
           u8g2.setCursor(27, 30);
          u8g2.print("Object"); 
          u8g2.setCursor(13, 55);
          u8g2.print("Tracking");
          if (enter == 1)
            {                     
              objectTracking();
            } 
              
          }
        
   
      } while (u8g2.nextPage());    
          
}



ISR(PCINT2_vect) {
  result = r.process();
  if (result == DIR_NONE) {
    
  }
  else if (result == DIR_CW && encPos < encMax) {
    encPos = encPos + encSteps;
  }
  else if (result == DIR_CCW && encPos > encMin) {
    encPos = encPos - encSteps;
  }
  
}

void click()
  {
  enter = 1;
  }

void slide()
{
 encPos = 0;
 enter = 0;
 while (enter == 0)
  {
  button.tick();
  encMin = -450;
  encMax = 450;
  encSteps = 25;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Distance"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("mm"); 
          

          slideDist = encPos;
          } while (u8g2.nextPage());
    
  }
    enter = 0;
    encPos = 10;

    
  while (enter == 0)
  {
  button.tick();
  encMin = 10;
  encMax = 3600;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Duration"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("sec"); 
          moveSpeed = slideDist / encPos;
          } while (u8g2.nextPage());
  } 
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 40);
          u8g2.print("Moving"); 
          
          } while (u8g2.nextPage());
  stepper1.setSpeedInMillimetersPerSecond(abs(moveSpeed));
  stepper1.setupRelativeMoveInMillimeters(slideDist);
    
  while(!stepper1.motionComplete()) 
        {
          stepper1.processMovement();
          
      
        }
  enter = 0;
  encPos = 0;
  moveSpeed = 0;
  slideDist = 0;
  
  }

  
void slideAndRotate()
{

 encPos = 0;
 enter = 0;
 while (enter == 0)
  {
  button.tick();
  encMin = -450;
  encMax = 450;
  encSteps = 25;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Distance"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("mm"); 
          slideDist = encPos;
          } while (u8g2.nextPage());
    
  }
    enter = 0;
    encPos = 0;
    
  while (enter == 0)
  {
  button.tick();
  encMin = -120;
  encMax =  120;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(0, 20);
          u8g2.print("Rot. Angle"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("deg"); 
          rotAngle = encPos/360;
          } while (u8g2.nextPage());
  }

        enter = 0;
        encPos = 10;
        
  while (enter == 0)
  {
  button.tick();
  encMin = 10;
  encMax = 3600;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Duration"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("sec"); 
          moveSpeed = slideDist / encPos;
          rotSpeed = rotAngle / encPos;
          } while (u8g2.nextPage());
  } 
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 40);
          u8g2.print("Moving"); 
          
          } while (u8g2.nextPage());
  stepper1.setSpeedInMillimetersPerSecond(abs(moveSpeed));
  stepper1.setupRelativeMoveInMillimeters(slideDist);
  stepper2.setSpeedInRevolutionsPerSecond(abs(rotSpeed));
  stepper2.setupRelativeMoveInRevolutions(rotAngle);
  
  while((!stepper1.motionComplete()|| !stepper2.motionComplete())) 
        {
          stepper1.processMovement();
          stepper2.processMovement();
        }
  
  
  enter = 0;
  encPos = 0;
  moveSpeed = 0;
  slideDist = 0;
  
   
  }



void slideAndRotateAndTilt()
{
  encPos = 0;
 enter = 0;
 while (enter == 0)
  {
  button.tick();
  encMin = -450;
  encMax = 450;
  encSteps = 25;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Distance"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("mm"); 
          slideDist = encPos;
          } while (u8g2.nextPage());
    
  }
    enter = 0;
    encPos = 0;
    
  while (enter == 0)
  {
  button.tick();
  encMin = -120;
  encMax =  120;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(0, 20);
          u8g2.print("Rot. Angle"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("deg"); 
          rotAngle = encPos/360;
          } while (u8g2.nextPage());
  }

        enter = 0;
        encPos = 0;

  while (enter == 0)
  {
  button.tick();
  encMin = -120;
  encMax =  120;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(0, 20);
          u8g2.print("Tilt. Angle"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("deg"); 
          tiltAngle = -encPos/360;
          } while (u8g2.nextPage());
  }

        enter = 0;
        encPos = 10;

        
  while (enter == 0)
  {
  button.tick();
  encMin = 10;
  encMax = 3600;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Duration"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("sec"); 
          moveSpeed = slideDist / encPos;
          rotSpeed = rotAngle / encPos;
          tiltSpeed = tiltAngle / encPos;
          } while (u8g2.nextPage());
  } 
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 40);
          u8g2.print("Moving"); 
          
          } while (u8g2.nextPage());
  stepper1.setSpeedInMillimetersPerSecond(abs(moveSpeed));
  stepper1.setupRelativeMoveInMillimeters(slideDist);
  stepper2.setSpeedInRevolutionsPerSecond(abs(rotSpeed));
  stepper2.setupRelativeMoveInRevolutions(rotAngle);
  stepper3.setSpeedInRevolutionsPerSecond(abs(tiltSpeed));
  stepper3.setupRelativeMoveInRevolutions(tiltAngle);
  
  
  while(!stepper1.motionComplete()|| !stepper2.motionComplete() || !stepper3.motionComplete())
        {
          stepper1.processMovement();
          stepper2.processMovement();
          stepper3.processMovement();
        }
  
  
  enter = 0;
  encPos = 0;
  moveSpeed = 0;
  slideDist = 0;
  tiltSpeed = 0;
  rotSpeed = 0;
  
  
   
  }  

void slideAndTilt()
{
  encPos = 0;
 enter = 0;
 while (enter == 0)
  {
  button.tick();
  encMin = -450;
  encMax = 450;
  encSteps = 25;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Distance"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("mm"); 
          slideDist = encPos;
          } while (u8g2.nextPage());
    
  }
    enter = 0;
    encPos = 0;
    
 

  while (enter == 0)
  {
  button.tick();
  encMin = -120;
  encMax =  120;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(0, 20);
          u8g2.print("Tilt. Angle"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("deg"); 
          tiltAngle = -encPos/360;
          } while (u8g2.nextPage());
  }

        enter = 0;
        encPos = 10;

        
  while (enter == 0)
  {
  button.tick();
  encMin = 10;
  encMax = 3600;
  encSteps = 10;
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 20);
          u8g2.print("Duration"); 
          u8g2.setCursor(15, 50);
          u8g2.print(encPos, 0);
          u8g2.setCursor(80, 50);
          u8g2.print("sec"); 
          moveSpeed = slideDist / encPos;
          tiltSpeed = tiltAngle / encPos;
          } while (u8g2.nextPage());
  } 
  u8g2.firstPage();
      do {
  
          u8g2.setFont(u8g2_font_osb18_tr);
          u8g2.setCursor(15, 40);
          u8g2.print("Moving"); 
          
          } while (u8g2.nextPage());
  stepper1.setSpeedInMillimetersPerSecond(abs(moveSpeed));
  stepper1.setupRelativeMoveInMillimeters(slideDist);
  stepper3.setSpeedInRevolutionsPerSecond(abs(tiltSpeed));
  stepper3.setupRelativeMoveInRevolutions(tiltAngle);
  
  
  while(!stepper1.motionComplete() || !stepper3.motionComplete())
        {
          stepper1.processMovement();
          
          stepper3.processMovement();
        }
  
  
  enter = 0;
  encPos = 0;
  moveSpeed = 0;
  slideDist = 0;
  tiltSpeed = 0;
  
  }  

void objectTracking()
{
  
  }    
  
