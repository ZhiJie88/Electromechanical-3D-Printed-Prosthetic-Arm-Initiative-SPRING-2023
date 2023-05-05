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

//State Denoter
uint8_t mode = 0;

//Pins for MG 90 servo motors in the hand
const uint8_t pointerPin = 31;
const uint8_t ringmiddlePin = 33;
const uint8_t pinkyPin = 29;
const uint8_t thumbPin = 35;

//Pins for MG 996R servo motors in the wrist
const uint8_t wristUDPin = 53;
const uint8_t wristLRPin = 51;

//Pins for DS3235 35KG servo motor in the elbow
const uint8_t elbowPin = 49;

//Variable representing direction and current servo positions
uint8_t pPos;
uint8_t pinkyPos;
uint8_t mPos;
uint8_t tPos;

//LED Pins
const uint8_t redPin = A1;
const uint8_t greenPin = A0;
const uint8_t bluePin = A2;

//Pressue sensor pin
const uint8_t fsrPin = A6;

bool upButState = false;
bool downButState = false;
bool leftButState = false;
bool rightButState = false;
bool startButState = false;
bool selectButState = false;
bool triangleButState = false;
bool squareButState = false;
bool crossButState = false;
bool circleButState = false;
bool stopGrip = false;
bool stopExtension = false;
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

  //Set mode to ajust wrist and elbow
  if (GamePad.isSelectPressed())
  {
    mode = 1;
  }

  //Set mode to adjust hand
  if (GamePad.isStartPressed())
  {
    mode = 2;
  }

  //Calls function to preform functionallity test for the hand
  if (mode == 0)
  {
    RGB(0, 255, 0);
    rigorMortis();
  }

  //Calls functions to control wrist and elbow
  else if (mode == 1)
  {
    RGB(255, 0, 0);
    adjustHand();
  }

  //Calls functions to control fingers
  else if (mode == 2)
  {
    RGB(0, 0, 255);
    adjustWristElbow();
  }

  //Reads values from pressure / force sensor
  uint16_t fsrVal = analogRead(fsrPin);
  Serial.print("FSR VAL:");
  Serial.println(fsrVal);
}

// Initialize all the servos for each finger, setting inital postions accordingly
void initFingers()
{
  pointer.attach(pointerPin);
  ringmiddle.attach(ringmiddlePin);
  thumb.attach(thumbPin);
  pinky.attach(pinkyPin);
  pointer.write(180);
  delay(20);
  ringmiddle.write(0); // The servo controlling the ring and middle finger is placed in the opposite orientation compared to the other finger servos
  delay(20);
  thumb.write(180);
  delay(20);
  pinky.write(180);
  delay(20); // Small delay to ensure the power supply does not encounter too much current draw at once
}

// Initialize all the servos for the wrist, setting inital postions accordingly
void initWrist()
{
  wristUD.attach(wristUDPin);
  wristLR.attach(wristLRPin);
  wristUD.write(60);
  wristLR.write(90);
  delay(100); // Small delay to ensure the power supply does not encounter too much current draw at once
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
  elbow.write(135);
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
  pinkyPos = pinky.read();
  Serial.println(pPos);
  if (pPos > 0)
  {
    pPos -= 2;
  }

  if (mPos < 180)
  {
    mPos += 2;
  }

  //pinky.write(pPos);
  ringmiddle.write(mPos);
  pointer.write(pPos);
  delay(20);
}

// Function to move the fingers out the palm (extention of fingers)
void release()
{
  pPos = pointer.read();
  mPos = ringmiddle.read();
  pinkyPos = pinky.read();
  Serial.println(pPos);

  if (pPos < 180)
  {
    pPos += 2;
  }

  if (mPos > 0)
  {
    mPos -= 2;
  }

  //pinky.write(pPos);
  ringmiddle.write(mPos);
  pointer.write(pPos);
  delay(20);
}

// Moves the thumb into the palm
void thumbpinkyAdjIn()
{
  tPos = thumb.read();
  
  if (tPos > 0)
  {
    tPos -= 2;
  }
  thumb.write(tPos);
  pinky.write(tPos);
  delay(20);
}

// Moves the thumb out of the plam
void thumbpinkyAdjOut()
{
  tPos = thumb.read();
  if (tPos < 180)
  {
    tPos += 2;
  }
  thumb.write(tPos);
  pinky.write(tPos);
  delay(20);
}

// Moves the hand up (palm facing the ground)
void wristUP()
{
  uint8_t wristUDpos = wristUD.read();
  if (wristUDpos < 170)
  {
    wristUDpos += 1;
  }
  wristUD.write(wristUDpos);
  delay(20);
}

// Moves the hand down (palm facing the ground)
void wristDOWN()
{
  uint8_t wristUDpos = wristUD.read();
  if (wristUDpos > 15)
  {
    wristUDpos -= 1;
  }
  wristUD.write(wristUDpos);
  delay(20);
}

// Moves the hand counterclockwise (ccw) (palm facing the ground)
void wristLEFT()
{
  uint8_t wristLRpos = wristLR.read();
  if (wristLRpos < 115)
  {
    wristLRpos += 1;
  }
  wristLR.write(wristLRpos);
  delay(20);
}

// Moves the hand clockwise (cw) (palm facing the ground)
void wristRIGHT()
{
  uint8_t wristLRpos = wristLR.read();
  if (wristLRpos > 0)
  {
    wristLRpos -= 1;
  }
  wristLR.write(wristLRpos);
  delay(20);
}

// Moves the arm down (parallel to the mount / table)
void elbowDOWN()
{
  uint8_t epos1 = elbow.read();
  if (epos1 < 215)
  {
    epos1 += 1;
  }
  elbow.write(epos1);
  delay(20);
}

// Moves the arm up (perpendicualr to the mount / table)
void elbowUP()
{

  uint8_t epos1 = elbow.read();
  if (epos1 > 0)
  {
    epos1 -= 1;
  }
  elbow.write(epos1);
  delay(20);
}

// grip egg
void gripEgg()
{
  uint16_t fsrVal = analogRead(fsrPin);
  pPos = pointer.read();

  // stops pnly when either the tolerance on the fsr is met or the servo can not grip any further
  while (fsrVal <= 300)
  {
    fsrVal = analogRead(fsrPin);
    pPos = pointer.read();
    while (thumb.read() > 2)
    {
      thumbpinkyAdjIn();
    }
    grip();
    if (pPos == 2)
    {
      break;
    }
  }
  //Serial.println("done");
}

// grip Cue ball
void gripCueBall()
{
  uint16_t fsrVal = analogRead(fsrPin);
  pPos = pointer.read();

  // stops when pnly either the tolerance on the fsr is met or the servo can not grip any further
  while (fsrVal <= 650)
  {
    fsrVal = analogRead(fsrPin);
    pPos = pointer.read();
    while (thumb.read() > 2)
    {
      thumbpinkyAdjIn();
    }
    grip();
    if (pPos == 2)
    {
      break;
    }
  }
}

// initial start up code to test that hand servos are connected correctly
void rigorMortis()
{
  if (GamePad.isUpPressed())
  {
    pinky.write(0);
  }
  if (GamePad.isDownPressed())
  {
    pinky.write(180);
  }
  if (GamePad.isLeftPressed())
  {
    ringmiddle.write(0);
  }
  if (GamePad.isRightPressed())
  {
    ringmiddle.write(180);
  }
  if (GamePad.isTrianglePressed())
  {
    pointer.write(0);
  }
  if (GamePad.isSquarePressed())
  {
    pointer.write(180);
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

  // Closes hand when triangle (v) is pressed
  if (GamePad.isDownPressed()) // && !triangleButState)
  {
    grip();
  }

  // Opens hand when cross (^) is pressed
  if (GamePad.isUpPressed())
  {
    release();
  }

  // Moves thumb away from palm when right (>) is pressed
  if (GamePad.isRightPressed() && !startButState)
  {
    thumbpinkyAdjOut();
  }

  // Moves thumb into the palm when left (<) is pressed
  if (GamePad.isLeftPressed())
  {
    thumbpinkyAdjIn();
  }

  // Grips egg when the triangle (∆) button is pressed
  if (GamePad.isTrianglePressed())
  {
    gripEgg();
  }

  // Grips cueball when the square (□) button is pressed
  if (GamePad.isSquarePressed())
  {
    gripCueBall();
  }
}