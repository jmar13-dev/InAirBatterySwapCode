//Code written by Jose A Martinez
//Assisted by Isaac Olson, Dr. David Hicks
//Written for In Air UAV Battery Swap Senior project

//Library Declarartion
#include <Servo.h>
#include <Encoder.h>

//Servo Declaration
Servo servo1; Servo servo2; Servo servo3; Servo servo4;

//pin Declatation
int led = 31; int channel2; const int landbutton = 30; const int fowardbutton = 36; const int backwardbutton = 40;
const int MA = 7; const int MB = 8;
int s1 = 5;//fourbar servo
int s2 = 4;//landing clamp servo
int s3 = 11;//continous servo
int s4 = 10;//key servo
//Hall Sensors
int LandingButton = 0; int BackwardButton = 0; int FowardButton = 0; const int RevHall = 44;

//Variables to be declared
int RevHallConfirm = 0; int RevolverHome = 1;
int Battery1 = 1; int Battery2 = 1; int Battery3 = 1; int Battery4 = 1; int Battery5 = 1; int Battery6 = 1; int battery = 0;
int nextBattery = 1;
int homie = 0;
boolean foundhome = 0;
int continoushome = 0; int revolverChange = 1; int revolverTurn = 1; int batterySwap = 1; int landingGate = 1;
int continousGate1 = 1; int continousGate2 = 1; int batteryStored = 1; int batteryGrab = 1; int newBatteryPull = 1;
int newBatteryIn = 1; int completedSwap = 1; int WaitforReceiver = 1;
//Offsets for Revolver
int o2 = 1667;
int p1 = 1450 - 130; int p2 = -40 + p1 + o2; int p3 = p2 + o2; int p4 = p3 + o2; int p5 = p4 + o2; int p6 = p5 + o2;
int FBD = 132; //four bar Default

//Activate encoder
Encoder myEnc(2, 3);

void setup() {
  Serial.begin(9600);
  Serial.print("Serial started...");
  pinMode(landbutton, INPUT);
  pinMode(backwardbutton, INPUT);
  pinMode(fowardbutton, INPUT);
  pinMode(MA, OUTPUT);
  pinMode(MB, OUTPUT);
  pinMode(RevHall, INPUT);

  pinMode(7, INPUT);
  pinMode(led, OUTPUT);

  //Check hall switch status
  BackwardButton = digitalRead(backwardbutton);
  FowardButton = digitalRead(fowardbutton);
  LandingButton = digitalRead(landbutton);
  //Set servo Attachments
  servo1.attach(s1); servo2.attach(s2); servo3.attach(s3); servo4.attach(s4);
  //Set Revlolver and coninous servo to home postition

  servo2.write(0);
  Serial.println("setting four bar to default position");
  servo1.write(FBD);
  Serial.println("Carriage going home...");
  servo4.write(45);
  servo3.write(100);
}
long oldPosition  = -999;
void loop() {

  channel2 = pulseIn(18, HIGH);

  LandingButton = digitalRead(landbutton);
  BackwardButton = digitalRead(backwardbutton);
  FowardButton = digitalRead(fowardbutton);

  RevHallConfirm = digitalRead(RevHall);
  long newPosition = myEnc.read();

  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
}
 //Continous Servo Finds Home
  if (BackwardButton == LOW && continoushome == 0) {
    Serial.println("Carriage is at home!");
    //Update the trigger for the next step
    continousGate1 = 0;
    servo3.write(92);
    continoushome = 1;
    RevolverHome = 0;
    delay(200);
    digitalWrite(MA, HIGH);
}
 //Revolver Finds Hom
  if (RevHallConfirm == LOW && RevolverHome == 0) {
    digitalWrite(MA, LOW);
    Serial.println("Found Home");
    homie = newPosition;
    myEnc.write(0);
    Serial.println("homie = ");
    Serial.print(homie);
    Serial.println("^stop here ");
    RevolverHome = 1;
    foundhome = 1;
    newPosition = 0;
    Battery1 = 0;
    delay(200);
}
 //Set Revolver postion to battery 1
  if (foundhome == 1 && Battery1 == 0) {
    digitalWrite(MA, HIGH);
    if (newPosition >= p1 ) { // position set 1
      digitalWrite(MA, LOW);
      Serial.println("Turn OFF");
      Battery1 = 1;
      landingGate = 0;
      delay(200);
      Serial.println("Waiting for Signal");
    }
}
 //Lock Drone
  if (LandingButton == HIGH && landingGate == 0) {
    Serial.println("Pin 2 activated");
    delay(1000);
    servo2.write(90);
    landingGate = 1;
    WaitforReceiver = 0;
    Serial.println("Waiting for Signal");
    digitalWrite(led, HIGH);
}
 //Wair for Signal to begin Swap
  if (channel2 >= 1800 && WaitforReceiver == 0) {
    digitalWrite(led, LOW);
    delay(200);
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
    servo1.write(30); // Move the servo to 37 degrees
    delay(500); // Wait for it to move to it's new position
    Serial.println("Fourbar is at the top");
    WaitforReceiver = 1;
    continousGate1 = 0;
    delay(1000);
    digitalWrite(led, HIGH);
    servo3.write(70);
}
 //Get ready to pull back Drone Battery 
  if (FowardButton == LOW && continousGate1 == 0) {
    Serial.println("Carriage is at the foward posistion");
    servo3.write(92);
    delay(200);
    servo4.write(90);
    delay(200);
    servo3.write(120);
    continousGate2 = 0;
    continousGate1 = 1;
}
 //Battery has been detached
  if (BackwardButton == LOW && continousGate2 == 0) {
    Serial.println("Carriage Came Back Home!");
    servo3.write(92);
    delay(200);
    continousGate2 = 1;
    continousGate1 = 1;
    batterySwap = 0;
}
 //FourBar Gets in position to store battery
  if (BackwardButton == LOW && batterySwap == 0) {
    servo1.write(157);
    delay(1000);
    servo3.write(70);
    delay(200);
    batterySwap = 1;
    batteryStored = 0;
}
 //Battery is Stored 
  if (FowardButton == LOW && batteryStored == 0) {
    servo3.write(92);
    delay(200);
    servo4.write(45);
    delay(200);
    servo3.write(120);
    batteryStored = 1;
    revolverChange = 0;
}
 //FourBar moves back to allow revolver to Turn
  if (BackwardButton == LOW && revolverChange == 0) {
    servo3.write(92);
    delay(200);
    servo1.write(FBD);
    delay(200);
    revolverChange = 1;
    nextBattery = 0;
    Battery2 = 0;
}
 //Move revolver to Battery 2
  if (battery == 0 && nextBattery == 0) {
    if (newPosition >= p1 && Battery2 == 0 ) { //restart motor after p1
      digitalWrite(MA, HIGH);
      if (newPosition >= p2 ) { // position set 2
        digitalWrite(MA, LOW);
        Serial.println("Turn OFF");
        delay(200);
        servo1.write(157);
        delay(1000);
        servo3.write(70);
        delay(200);
        Battery2 = 1;
        Battery3 = 0;
        batteryGrab = 0;
        nextBattery = 1;
      }
    }
}
 //IF FIRST SWAP HAS HAPPENED THIS IS FOR SECOND SWAP
  if (battery == 1 && nextBattery == 0) {
    if (newPosition >= p2 && Battery3 == 0 ) { //restart motor after p2
      digitalWrite(MA, HIGH);
      if (newPosition >= p3 ) { // position set 3
        digitalWrite(MA, LOW);
        Serial.println("Battery 3");
        delay(200);
        servo1.write(157);
        delay(1000);
        servo3.write(70);
        delay(200);
        Battery3 = 1;
        Battery4 = 0;
        batteryGrab = 0;
        nextBattery = 1;
      }
    }
}
 //IF SECOND SWAP HAS HAPPENED THIS IS FOR THIRD SWAP
  if (battery == 2 && nextBattery == 0) {
    if (newPosition >= p3 && Battery4 == 0 ) { //restart motor after p3
      digitalWrite(MA, HIGH);
      if (newPosition >= p4 ) { // position set 4
        digitalWrite(MA, LOW);
        Serial.println("Battery 4");
        delay(200);
        servo1.write(157);
        delay(1000);
        servo3.write(70);
        delay(200);
        Battery4 = 1;
        Battery5 = 0;
        batteryGrab = 0;
        nextBattery = 1;
      }
    }
}
 //IF THIRD SWAP HAS HAPPENED THIS IS FOR FOURTH SWAP
  if (battery == 3 && nextBattery == 0) {
    if (newPosition >= p4 && Battery5 == 0 ) { //restart motor after p4
      digitalWrite(MA, HIGH);
      if (newPosition >= p5 ) { // position set 5
        digitalWrite(MA, LOW);
        Serial.println("Battery 5");
        delay(200);
        servo1.write(157);
        delay(1000);
        servo3.write(70);
        delay(200);
        Battery5 = 1;
        Battery6 = 0;
        batteryGrab = 0;
        nextBattery = 1;
      }
    }
}
 //IF FOUTH SWAP HAS HAPPENED THIS IS FOR FIFTH SWAP
  if (battery == 4 && nextBattery == 0) {
    if (newPosition >= p5 && Battery6 == 0 ) { //restart motor after p4
      digitalWrite(MA, HIGH);
      if (newPosition >= p6 ) { // position set 6
        digitalWrite(MA, LOW);
        Serial.println("Battery 6");
        delay(200);
        servo1.write(157);
        delay(1000);
        servo3.write(70);
        delay(200);
        Battery6 = 1;
        batteryGrab = 0;
        nextBattery = 1;
      }
    }
}
 //Sets fourbar back to position to pullout new battery 
  if (FowardButton == LOW && batteryGrab == 0) {
    servo3.write(92);
    delay(200);
    servo4.write(90);
    delay(200);
    batteryGrab = 1;
    newBatteryPull = 0;
    servo3.write(120);
    delay(200);
}
 //Pulls out battery and set fourbar to insert ready position
  if (BackwardButton == LOW && newBatteryPull == 0) {
    servo3.write(92);
    delay(200);
    servo1.write(30);
    delay(1000);
    servo3.write(70);
    delay(200);
    newBatteryPull = 1;
    newBatteryIn = 0;
}
 //Inserts battery into drone
  if (FowardButton == LOW && newBatteryIn == 0 ) {
    servo3.write(92);
    delay(200);
    servo4.write(45);
    delay(200);
    newBatteryIn = 1;
    completedSwap = 0;
    servo3.write(120);
    delay(200);
}
 //Sets fourbar back to ready position and Adds to battery counter to use next battery
  if (BackwardButton == LOW && completedSwap == 0) {
    servo3.write(92);
    delay(200);
    servo1.write(FBD);
    delay(500);
    completedSwap = 1;
    servo2.write(0);
    digitalWrite(led, LOW);
    Serial.println("AND ITS COMPLETE");
    delay(5000);
    landingGate = 0;
    battery = battery + 1;
    Serial.println("We are at battery:");
    Serial.println(battery);
  }
}
