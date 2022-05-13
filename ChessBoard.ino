          
/*Author Miran Manasrah 
 * Version 1.0 May 12th 2022
 * This code was developed to read voltage readings
  from 64 Hall effect sensors and process the readings
  to determine the move that was made on the chessboard*/

#include <LiquidCrystal_I2C.h>

// MUX digital selection pins
#define S0 3
#define S1 4
#define S2 5
#define S3 6
//MUX analog output pins
int SIG[4] = {A0, A1, A2, A3};

//Buttons digital pins
int red_button = 8;
int green_button = 9;
int blue_button = 10;
//Button initialization
int red_state = 0;
int green_state = 0;
int blue_state = 0;

//LCD Address
LiquidCrystal_I2C lcd(0x20,16,2);

int numPush = 0; // keeps track of green button
int sensor; //analog reading of hall effect sensor

//Arrays to save readings
int outArray1[64];
int outArray2[64];

//Initialize previous and current position 
String previousPosition ="";
String currentPosition ="";
String transition;//value communicated to rasberry pi

int start_flag = 0; //flag to indicate if a game has started
String correct_check = "";
bool correct_positions = false;

char* chessPosition[64] = {
  "a8", "a7", "a6", "a5", "a4", "a3", "a2", "a1",
  "b8", "b7", "b6", "b5", "b4", "b3", "b2", "b1",
  "c8", "c7", "c6", "c5", "c4", "c3", "c2", "c1",
  "d8", "d7", "d6", "d5", "d4", "d3", "d2", "d1",
  "e8", "e7", "e6", "e5", "e4", "e3", "e2", "e1",
  "f8", "f7", "f6", "f5", "f4", "f3", "f2", "f1",
  "g8", "g7", "g6", "g5", "g4", "g3", "g2", "g1",
  "h8", "h7", "h6", "h5", "h4", "h3", "h2", "h1"
};

void setup(){
  //Arrays initialization
  int outArray1[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int outArray2[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //LCD initialization
  lcd.init();
  lcd.clear();
  lcd.backlight();
                      
  Serial.begin(9600);
  
  pinMode(S0, OUTPUT); 
  pinMode(S1, OUTPUT); 
  pinMode(S2, OUTPUT); 
  pinMode(S3, OUTPUT);   

  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  pinMode(red_button, OUTPUT);
  pinMode(green_button, OUTPUT);
  pinMode(blue_button, OUTPUT);

  //Wait for game to start
  if (start_flag == 0){
    wait();
  }

  //Start game when RPi sends a message
  while(!correct_positions) {
    sendInitialReadings();
    correct_check = serialReceive();

    if(correct_check.equals("Game Starting")){
      correct_positions = true;
    }
  }
}

void loop(){
  
  green_state = digitalRead(green_button);
  red_state = digitalRead(red_button);

  //Check if RPi sent message 
  serialReceive();
  //When green button is pushed
  if (green_state == HIGH){
      humanPlay();
      delay(5);
   }
    //When red button is pushed
   else if(red_state == HIGH){
      endGame();
   }
    delay(5);   
}
//Waits until blue button is pressed to start game
void wait(){
  while(start_flag == 0){
    blue_state = digitalRead(blue_button);
    if(blue_state == HIGH){
      lcdPrint("Blue Pressed","");
      start_flag = 1;
    }
    delay(5);
  }
}
//Ends game
void endGame(){
  start_flag = 0;
  delay(5);
}
//Sends initial readings to RPi to check initial positions of pieces
void sendInitialReadings(){
  int initialArray[64] = {0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0};
   int num1 = 0;
        for (int mux = 0; mux<4; mux++){
                sensor = SIG[mux];
                for(int i = 0; i < 16; i ++){
                  initialArray[num1] = readMux(i);
                  serialTransmit(initialArray[num1]);
                  num1++;
                }
              }
              delay(5);                  
}
//Checks if RPi sent anything
String serialReceive(){
    String data = "";
    if (Serial.available() > 0) {
      data = Serial.readStringUntil('\n');
      //Displays message pf RPi on lCD
      lcd.clear();
      lcdPrint(data,"");
    }
    return data;
  }
  
void humanPlay(){   
    int num = 0; //keeps track of the total number of sensors to read
    //First push saves readings in Array
    if (numPush < 1){
          lcd.clear();
          lcdPrint("First Push","Make a Move!");
          for (int mux = 0; mux<4; mux++){
            sensor = SIG[mux];
            for(int i = 0; i < 16; i ++){
              outArray2[num] = readMux(i);
              serialTransmit(outArray2[num]);             
              num++;
            }
          }
          delay(10);
          numPush++;
        }
      //Second push saves readings in Array  
      else{
          lcd.clear();
          lcdPrint("Second Push","");
          for (int mux = 0; mux<4; mux++){
            sensor = SIG[mux];
            for(int i = 0; i < 16; i ++){
              outArray1[num] = readMux(i);
              serialTransmit(outArray1[num]);
              num++;
            }
          }
          delay(10);
          numPush = 0;
          compare();
        }
}

//Reads MUX
int readMux(int channel){
        int controlPin[] = {S0, S1, S2, S3};
        int muxChannel[16][4]={
          {0,0,0,0}, //channel 0
          {1,0,0,0}, //channel 1
          {0,1,0,0}, //channel 2
          {1,1,0,0}, //channel 3
          {0,0,1,0}, //channel 4
          {1,0,1,0}, //channel 5
          {0,1,1,0}, //channel 6
          {1,1,1,0}, //channel 7
          {0,0,0,1}, //channel 8
          {1,0,0,1}, //channel 9
          {0,1,0,1}, //channel 10
          {1,1,0,1}, //channel 11
          {0,0,1,1}, //channel 12
          {1,0,1,1}, //channel 13
          {0,1,1,1}, //channel 14
          {1,1,1,1}  //channel 15
        };
        //loop through the 4 sig
        for(int i = 0; i < 4; i ++){
          digitalWrite(controlPin[i], muxChannel[channel][i]);
        }
        //read the value at the SIG pin
        int val = analogRead(sensor);
        //return the value
        return val;
}

void compare(){
    currentPosition = "";
    previousPosition = "";
    for(int y=0; y<64; y++){
      if(abs(outArray2[y] - outArray1[y]) > 150){
        //In the case of occupying an empty square with NO CAPTURING
        if(outArray1[y] > 200 && outArray1[y] < 300){
            previousPosition = chessPosition[y];
        }
        else if(outArray2[y] > 200 && outArray2[y] < 300){
            currentPosition = chessPosition[y];
        }
        //In the case of occupying a square with CAPTURING a robot's piece
        else if(abs(outArray2[y] - outArray1[y]) > 400){
          currentPosition = chessPosition[y];              
        }
      }
    }
    //Construct the transition string
    if(previousPosition == "" || currentPosition == ""){
      //DO NOTHING
    }
    else{
        transition = previousPosition+currentPosition;              
    }
      serialTransmit(transition);  
}
//Transmits Data to Raspberry Pi
void serialTransmit(String data){
      Serial.println(data);
}
//Displays messages on LCD
void lcdPrint(String line1, String line2){
      lcd.setCursor(0,0);
      lcd.print(line1);
      lcd.setCursor(0,1);
      lcd.print(line2);
}
