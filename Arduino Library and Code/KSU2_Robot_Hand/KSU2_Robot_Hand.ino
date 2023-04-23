// Set refrence and varibles for dabble bluetooth interface
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

//Set reference to servo library
#include <Servo.h>

//Variables to reference the servo library
Servo pointer;
Servo ringmiddle;
Servo thumb;
Servo pinky;
Servo wristUD;
Servo wristLR;
Servo elbow;
Servo elbow2;

//Pins for MG 90 servo motors in the hand
const uint8_t pointerPin = 29;
const uint8_t ringmiddlePin = 31;
const uint8_t pinkyPin = 33;
const uint8_t thumbPin = 35;

//Pins for MG 996R servo motors in the wrist
const uint8_t wristUDPin = 53;
const uint8_t wristLRPin = 51;

//Pins for DS3235 35KG servo motor in the elbow
const uint8_t elbowPin = 50;

//Variable representing direction and current servo positions
uint8_t pPos;
uint8_t mPos;
uint8_t tPos;

//LED Pins
const uint8_t redPin = A1;
const uint8_t greenPin = A0;
const uint8_t bluePin = A2;

//Pressue sensor pin
const uint8_t fsrPin = A6;

//Inital startup for Arduino
void setup()
{
  //Defines a baud rate to communicate between the Serial Monitor and the Arduino to aid in debugging
  Serial.begin(115200);

  //Defines baud rate to communicate with the HM-10 Bluetooth Module to communicate with the Dabble App Interface
  Dabble.begin(9600);

  //Initialize servos for the fingers, wrist, and elbows
  initFingers();
  initWrist();
  initElbow();

  //Initialize pin for sensor
  initSensor();

  //Initialize pins RGB lighting
  initRGB();
}

//Arduino main looping function, repeated until Arduino is powered off
void loop()
{
  //Allows the Arduino to process information from the Dabble App
  Dabble.processInput();

  //Calls functions to control wrist and elbow
  adjustWristElbow();

  //Calls functions to control fingers
  adjustHand();

  //Reads values from pressure / force sensor
  int fsrVal = analogRead(fsrPin);
  Serial.print("FSR VAL:");
  Serial.println(fsrVal);

  // Blink red at egg grip values are met
  if (fsrVal >= 220 && fsrVal <= 250)
  {
    RGB(255, 0, 0);
    delay(100);
    RGB(0, 0, 0);
    delay(100);
  }

  // Blink blue at billiard grip values are met
  else if (fsrVal >= 600 && fsrVal <= 750)
  {
    RGB(0, 0, 255);
    delay(20);
    RGB(0, 0, 0);
    delay(100);
  }

  // Otherwisre, turn LED off
  else
  {
    RGB(0, 0, 0);
  }
}

// Initialize all the servos for each finger, setting inital postions accordingly
void initFingers()
{
  pointer.attach(pointerPin);
  ringmiddle.attach(ringmiddlePin);
  thumb.attach(thumbPin);
  pinky.attach(pinkyPin);
  pointer.write(180);
  ringmiddle.write(0);  // The servo controlling the ring and middle finger is places in the opposite orientation compared to the other finger servos
  thumb.write(180);
  pinky.write(180);
  delay(100);   // Small delay to ensure the power supply does not encounter too much current draw at once
}

// Initialize all the servos for the wrist, setting inital postions accordingly
void initWrist()
{
  wristUD.attach(wristUDPin);
  wristLR.attach(wristLRPin);
  wristUD.write(60);
  wristLR.write(0);
  delay(100);   // Small delay to ensure the power supply does not encounter too much current draw at once
}

// Initialize all the led pins to output
void initRGB()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

// Initialize all the pressure sensor pins to input
void initSensor()
{
  pinMode(fsrPin, INPUT);
}

// Initialize elbow servo pin, setting initial value to being perpendicular to the wood plank
void initElbow()
{
  elbow.attach(elbowPin);
  elbow.write(0);
}

// Function to control the color of the LED as either red, green, or blue (RGB)
void RGB(uint8_t redValue, uint8_t greenValue, uint8_t blueValue)
{
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

// Function to move the fingers into the palm (grip)
void grip()
{
  pPos = pointer.read();
  mPos = ringmiddle.read();
  mPos += 10;
  pPos -= 10;
  if (pPos != 180)
  {
    pinky.write(pPos);
    delay(15); // small delay to allow the pinky to actuate fist and assist and stabilizing the object
    ringmiddle.write(mPos);
    pointer.write(pPos);
  }
}

// Function to move the fingers out the palm (extention of fingers)
void release()
{
  pPos = pointer.read();
  mPos = ringmiddle.read();
  mPos -= 10;
  pPos += 10;
  if (pPos != 0)
  {
    pinky.write(pPos);
    delay(15); // small delay to allow the pinky to actuate fist and assist and stabilizing the object
    ringmiddle.write(mPos);
    pointer.write(pPos);
  }
}

// Moves the thumb into the palm
void thumbAdjIn()
{
  tPos = thumb.read();
  tPos -= 10;
  if (tPos != 180)
  {
    thumb.write(tPos);
  }
}

// Moves the thumb out of the plam
void thumbAdjOut()
{
  tPos = thumb.read();
  tPos += 10;
  if (tPos != 0)
  {
    thumb.write(tPos);
  }
}

// Moves the hand up (palm facing the ground)
void wristUP()
{
  int wristUDpos = wristUD.read();
  if (wristUDpos < 170)
  {
    wristUDpos += 15;
    wristUD.write(wristUDpos);
  }
}

// Moves the hand down (palm facing the ground)
void wristDOWN()
{
  int wristUDpos = wristUD.read();
  if (wristUDpos > 15)
  {
    wristUDpos -= 15;
    wristUD.write(wristUDpos);
  }
}

// Moves the hand counterclockwise (ccw) (palm facing the ground)
void wristLEFT()
{
  int wristLRpos = wristLR.read();
  if (wristLRpos < 115)
  {
    wristLRpos += 15;
    wristLR.write(wristLRpos);
  }
}

// Moves the hand clockwise (cw) (palm facing the ground)
void wristRIGHT()
{
  int wristLRpos = wristLR.read();
  if (wristLRpos > 0)
  {
    wristLRpos -= 15;
    wristLR.write(wristLRpos);
  }
}

// Moves the arm down (parallel to the mount / table)
void elbowDOWN()
{
  int epos1 = elbow.read();
  if (epos1 != 80)
  {
    epos1 += 10;
    elbow.write(epos1);
  }
}

// Moves the arm up (perpendicualr to the mount / table)
void elbowUP()
{

  int epos1 = elbow.read();
  if (epos1 != 0)
  {
    epos1 -= 10;
    elbow.write(epos1);
  }
}

// Function to check buttons for moving wrist and elbow
void adjustWristElbow()
{
  Dabble.processInput(); // Process button clicks from Dabble App

  // Moves the arm up when square (□) is pressed
  if (GamePad.isSquarePressed())
  {
    elbowUP();
  }

  // Moves the arm down when circle (o) is pressed
  if (GamePad.isCirclePressed())
  {
    elbowDOWN();
  }

  // Moves the hand up when up (^) is pressed (palm facing down)
  if (GamePad.isUpPressed())
  {
    wristUP();
  }

  // Moves the hand down when down (v) is pressed (palm facing down)
  if (GamePad.isDownPressed())
  {
    wristDOWN();
  }

  // Moves the hand ccw when left (<) is pressed (palm facing down)
  if (GamePad.isLeftPressed())
  {
    wristLEFT();
  }

  // Moves the hand cw when right (>) is pressed (palm facing down)
  if (GamePad.isRightPressed())
  {
    wristRIGHT();
  }
}

// Function to check Dabble buttons for moving fingers and thumb
void adjustHand()
{
  Dabble.processInput(); // Process button clicks from Dabble App

  // Closes hand when triangle (∆) is pressed
  if (GamePad.isTrianglePressed())
  {
    grip();
  }

  // Opens hand when cross (x) is pressed
  if (GamePad.isCrossPressed())
  {
    release();
  }

  // Moves thumb away from palm when START is pressed
  if (GamePad.isStartPressed())
  {
    thumbAdjOut();
  }

  // Moves thumb into the palm when SELECT is pressed
  if (GamePad.isSelectPressed())
  {
    thumbAdjIn();
  }
}