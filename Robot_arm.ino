
//Author Samira Souley Hassane
//Version 1.0 May 12th 2022
//This code controls a Scarra robot arm to pick up and drop chess pieces 

#include <AccelStepper.h>             //libary to control stepper motors
#include <Servo.h>
#include <math.h>
                  

// Create stepper motor 
AccelStepper xStepper(1, 2, 5);     // (Type:driver, STEP, DIR)
AccelStepper yStepper(1, 3, 6);
AccelStepper zStepper(1, 4, 7);

const byte enablePin = 8;           //stepper enable/disable pin

//Limit switches 
#define yLimitSwitch 10
#define xLimitSwitch 9
#define zLimitSwitch A3

//Gripper
#define SERVO 11
Servo gripper;                  // servo motor for the gripper

int x;
int y;

int xval;
int yval;
int zval;

//Time values 
unsigned long startTime;
unsigned long endTime;
unsigned long timeTaken;

void openGripper() {
  gripper.attach(11);
  gripper.write(0);
  delay(300);
}

void closeGripper(int delayTime) {
  gripper.attach(11);
  gripper.write(180);
  delay(delayTime);
}

void gripperHome(){
  closeGripper(20);
}

void stopServo() {
  gripper.write(90);
  delay(2000);
}

void pickUp(int x, int y) {
 allHome();
 
 xStepper.moveTo(x);
 yStepper.moveTo(y);
 while(xStepper.currentPosition() != x || yStepper.currentPosition() != y){
    if(xStepper.currentPosition() != x ){
       xStepper.run();
    }
    if(yStepper.currentPosition() != y){
      yStepper.run();
    }
  }
  
  openGripper();
  stopServo();
  
  zStepper.runToNewPosition(-2600);
  
  closeGripper(300);
  stopServo();
  
  resetZAxis();
}

void drop(int x, int y) {
  allHome(); 
  
  xStepper.moveTo(x);
  yStepper.moveTo(y);
  
 while(xStepper.currentPosition() != x || yStepper.currentPosition() != y){
    if(xStepper.currentPosition() != x ){
       xStepper.run();
    }
    if(yStepper.currentPosition() != y){
      yStepper.run();
    }
  }
  
  zStepper.runToNewPosition(-2400);
  
  openGripper();
  stopServo();
  resetZAxis();
  
  closeGripper(350);
  stopServo();
}

void xHome() {
  xStepper.setCurrentPosition(0);
  xStepper.setSpeed(2000);
  xStepper.setAcceleration(1500);
  
  int x_initial = 10; 
  
  while (!digitalRead(xLimitSwitch)) {           // Make the xStepper move CCW until the switch is activated
      xStepper.moveTo(x_initial);               // Set the position to move to
      //xStepper.runToNewPosition(x_initial);
      x_initial  += 50;                              // increment by 1 for next move if needed
      xStepper.run();                           // motor stepper motor   
      delay(25);
  }
  xStepper.setAcceleration(10);
  xStepper.setCurrentPosition(0);
  xStepper.runToNewPosition(-1000);
  xStepper.setCurrentPosition(0);
}

void yHome() {
  yStepper.setCurrentPosition(0);
  yStepper.setSpeed(1500);
  int y_initial = 1; 
  
  while (!digitalRead(yLimitSwitch)) {           // Make the xStepper move CCW until the switch is activated
      yStepper.moveTo(y_initial);               // Set the position to move to
      y_initial++;                                // Decrease by 1 for next move if needed
      yStepper.run();                           // motor stepper motor 
      delay(5);
   }
  
    yStepper.setCurrentPosition(0);
    yStepper.runToNewPosition(-1380);
    yStepper.setCurrentPosition(0);
}

void resetZAxis() {
   zStepper.setCurrentPosition(0);
  
  zStepper.setSpeed(1000);
  int z_initial = -10; 
  
  while (!digitalRead(zLimitSwitch)) {           
      zStepper.moveTo(z_initial);               
      z_initial += 5;                                
      zStepper.run();                          
      delay(5);
   }
  
    zStepper.setCurrentPosition(0);
    zStepper.runToNewPosition(-20); 
    zStepper.setCurrentPosition(0);
}

//up positive
//down negative 
void zHome() {
  zStepper.setCurrentPosition(0);
  
  zStepper.runToNewPosition(-400);
  zStepper.setSpeed(1000);
  int z_initial = -10; 
  
  while (!digitalRead(zLimitSwitch)) {           
      zStepper.moveTo(z_initial);               
      z_initial += 5;                                
      zStepper.run();                          
      delay(5);
   }
  
    zStepper.setCurrentPosition(0);
    
    zStepper.runToNewPosition(-280); // change this value to change the home position
    zStepper.setCurrentPosition(0);  
}

void allHome(){
  xStepper.setCurrentPosition(0);
  xStepper.setSpeed(2000);
  xStepper.setAcceleration(1500);
  int x_initial = 10;
   
  yStepper.setCurrentPosition(0);
  yStepper.setSpeed(1500);
  int y_initial = 1; 
  
  zStepper.setCurrentPosition(0);
  //zStepper.runToNewPosition(-200);
  zStepper.setSpeed(1000);
  int z_initial = -10; 

  while(!digitalRead(zLimitSwitch) || !digitalRead(yLimitSwitch) || !digitalRead(xLimitSwitch)){
    if(!digitalRead(zLimitSwitch)){
      zStepper.moveTo(z_initial);               
      z_initial += 5;                                
      zStepper.run();                          
      delay(5);
    }
    
   if(!digitalRead(yLimitSwitch)){
      yStepper.moveTo(y_initial);               // Set the position to move to
      y_initial++;                              // Decrease by 1 for next move if needed
      yStepper.run();                           // motor stepper motor 
      delay(5);
  }
  
  if(!digitalRead(xLimitSwitch)){
      xStepper.moveTo(x_initial);               // Set the position to move to
      x_initial  += 50;                         // increment by 1 for next move if needed
      xStepper.run();                           // motor stepper motor   
      delay(25);
  }
  
  }
  
  zStepper.setCurrentPosition(0);
  yStepper.setCurrentPosition(0);
  xStepper.setCurrentPosition(0);
  
  xStepper.setAcceleration(10);
  
  xval = -1000;
  yval = -1380;
  zval = -200;

  xStepper.moveTo(xval);
  yStepper.moveTo(yval);
  zStepper.moveTo(zval); // change this value to change the home position
 
  
 while(xStepper.currentPosition() != xval || yStepper.currentPosition() != yval || zStepper.currentPosition() != zval){
    if(xStepper.currentPosition() != xval ){
       xStepper.run();
    }
    
    if(yStepper.currentPosition() != yval){
      yStepper.run();
    }

    if(zStepper.currentPosition() != zval){
      zStepper.run();
    }
    
  }
  
  zStepper.setCurrentPosition(0);  
  yStepper.setCurrentPosition(0);
  xStepper.setCurrentPosition(0);
   
}

void setup() {
  
  // Enable pin for the CNC shield 
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);

  // Stepper motors speed and acceleration
  xStepper.setMaxSpeed(2000);
  xStepper.setSpeed(1000);

  yStepper.setMaxSpeed(2000);
  yStepper.setSpeed(1500);
  yStepper.setAcceleration(600);
 
  zStepper.setMaxSpeed(2000);
  zStepper.setSpeed(1000);
  zStepper.setAcceleration(100);

  //Gripper servo motor 
  pinMode(SERVO, OUTPUT);

  //Limit switches 
  pinMode(xLimitSwitch, INPUT_PULLUP);
  pinMode(yLimitSwitch, INPUT_PULLUP);
  pinMode(zLimitSwitch, INPUT_PULLUP);
  
  //serial 
  Serial.begin(9600);


  //Test coordinates on the chessboard 
  pickUp(470,-950);
  xStepper.runToNewPosition(-400);

  openGripper();
  stopServo();
  closeGripper(300);
  stopServo();
  
  pickUp(-95,-1370);
  drop(470,-950);
  
}

void loop() {
}
