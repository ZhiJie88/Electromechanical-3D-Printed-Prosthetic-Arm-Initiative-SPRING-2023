#include <DabbleESP32.h>
#include <ESP32Servo.h>
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

const int wristPin1 = 27;
const int wristPin2 = 26;
const int pointerPin = 32;
const int pinkyPin = 33;
const int thumbPin = 14;
const int forearmPin = 25;
const int elbowPin = 13;

const int sensorPin = 35;
int analogReading = 0;
int oldReading = 0;
const int cuePressure = 1200; // Values found through trial and error, may change if ground 
const int eggPressure = 400;  // resistor value is changed. (see FDR: Force Feedback for more info)

bool ArmControl = false; // if FALSE, no arm control
bool HandControl = false; // if FALSE, no direct hand control

int elbowPos = 0;
int pointerPos = 145;
int pinkyPos = 145;
int forearmPos = 180;
int thumbPos = 165;

int minUS = 1000;
int maxUS = 2000;

Servo pointer;
Servo pinky;
Servo thumb;
Servo forearm;
Servo elbow;

void setup()
{
  Serial.begin(115200);
  Dabble.begin("KSU1 ARM");
  pinMode(wristPin1, OUTPUT);
  pinMode(wristPin2, OUTPUT);

// Following code sets up each frequency for the servos. This is unique
// to the ESPServo library, and can be replaced if a new boards is used.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  pointer.setPeriodHertz(50);
  pinky.setPeriodHertz(50);
  thumb.setPeriodHertz(50);
  forearm.setPeriodHertz(50);
  elbow.setPeriodHertz(50);

// The following code sets up the timers for each servo, and again is
// unique to ESPServo. The only unique servo is the MG90, which uses
// different minimum and maximum timings. See online resources for more
// assistance in setting the timings.
  pointer.attach(pointerPin, minUS, maxUS);
  pinky.attach(pinkyPin, minUS, maxUS);
  thumb.attach(thumbPin, 400, 2400);
  forearm.attach(forearmPin, minUS, maxUS);
  elbow.attach(elbowPin, minUS, maxUS);

  pointer.write(pointerPos);
  pinky.write(pinkyPos);
  thumb.write(thumbPos);
  forearm.write(forearmPos);
  elbow.write(elbowPos);
}

void loop()
{
  Dabble.processInput();

  // Mode Selection options: 
  //   The controller swaps between controlling the arm and the
  //   hand to effectively make use of all the dabble buttons.
  if (GamePad.isStartPressed())
  {
    Serial.println("START");
    HandControl = true;
    ArmControl = false;
    delay(200);
  }
  else if (GamePad.isSelectPressed())
  {
    Serial.println("SELECT");
    HandControl = false;
    ArmControl = true;
    delay(200);
  }

  // Hand Control
  if (HandControl == true)
  {
    // Pointer & Middle finger control
    if (GamePad.isUpPressed())
    {
      //Serial.println("UP");
      if (pointerPos <= 145)
      {
        pointerPos += 3;
      }
      pointer.write(pointerPos);
      analogReading = analogRead(sensorPin);
      Serial.print("reading: ");
      Serial.println(analogReading);
      delay(25);
    }
    else if (GamePad.isDownPressed())
    {
      //Serial.println("DOWN");
      if (pointerPos > 40)
      {
        pointerPos -= 3;
      }
      pointer.write(pointerPos);
      analogReading = analogRead(sensorPin);
      Serial.print("reading: ");
      Serial.println(analogReading);
      delay(20);
    }

    // Ring and Pinky finger control
    else if (GamePad.isTrianglePressed())
    {
      Serial.println("TRIANGLE");
      if (pinkyPos <= 145)
      {
        pinkyPos += 3;
      }
      pinky.write(pinkyPos);
      delay(20);
    }
    else if (GamePad.isCrossPressed())
    {
      Serial.println("CROSS");
      if (pinkyPos > 40)
      {
        pinkyPos -= 3;
      }
      pinky.write(pinkyPos);
      delay(25);
    }

    // Thumb Control
    else if (GamePad.isLeftPressed())
    {
      Serial.println("LEFT");
      if (thumbPos <= 165)
      {
        thumbPos += 1;
      }
      thumb.write(thumbPos);
      Serial.println(thumbPos);
      delay(10);
    }
    else if (GamePad.isRightPressed())
    {
      Serial.println("RIGHT");
      if (thumbPos > 65)
      {
        thumbPos -= 1;
      }
      thumb.write(thumbPos);
      Serial.println(thumbPos);
      delay(10);
    }
    // All finger Control
    else if (GamePad.isSquarePressed())
    {
      Serial.println("SQUARE");
      pointerPos = pinkyPos;
      if (pointerPos <= 145)
      {
        pointerPos += 3;
        pinkyPos += 3;
      }
      pointer.write(pointerPos);
      pinky.write(pinkyPos);
      delay(25);
    }
    else if (GamePad.isCirclePressed())
    {
      Serial.println("CIRCLE");
      pointerPos = pinkyPos;
      if (pointerPos > 40)
      {
        pointerPos -= 3;
        pinkyPos -= 3;
      }
      pointer.write(pointerPos);
      pinky.write(pinkyPos);
      delay(25);
    }
  }

  // Arm Control
  else if (ArmControl == true)
  {
    // Elbow Control
    if (GamePad.isUpPressed())
    {
      Serial.println("UP");
      if (elbowPos <= 180)
      {
        elbowPos += 1;
      }
      elbow.write(elbowPos);
      delay(20);
    }
    else if (GamePad.isDownPressed())
    {
      Serial.println("DOWN");
      if (elbowPos > 0)
      {
        elbowPos -= 1;
      }
      elbow.write(elbowPos);
      delay(20);
    }

    // Forearm Control
    else if (GamePad.isLeftPressed())
    {
      Serial.println("LEFT");
      if (forearmPos <= 180)
      {
        forearmPos += 1;
      }
      Serial.println(forearmPos);
      forearm.write(forearmPos);
      delay(20);
    }
    else if (GamePad.isRightPressed())
    {
      Serial.println("RIGHT");
      if (forearmPos > 0)
      {
        forearmPos -= 1;
      }
      Serial.println(forearmPos);
      forearm.write(forearmPos);
      delay(20);
    }

    // Grab Stuff:
    //   The following two commands utilize the "pressure" reading to detect
    //   when an object is grabbed. See the FDR for further explaination on
    //   the pressure sensor functions. Note that a lower value of pressure
    //   results in a lower output force at the fingertips.
    else if (GamePad.isCirclePressed())
    {
      pointerPos = 145;
      pinkyPos = 145;
      pointer.write(pointerPos);
      pinky.write(pinkyPos);
      //oldReading = analogRead(sensorPin);
      analogReading = analogRead(sensorPin);
      while(analogReading < eggPressure){
        pointerPos -= 1;
        pinkyPos -= 1;
        pointer.write(pointerPos);
        pinky.write(pinkyPos);
        analogReading = analogRead(sensorPin);
        Serial.println(analogReading);
        delay(20); // Without the delay, the arm gets stuck in the while loop
      }
    }
    else if (GamePad.isSquarePressed())
    {
      pointerPos = 145;
      pinkyPos = 145;
      pointer.write(pointerPos);
      pinky.write(pinkyPos);
      //oldReading = analogRead(sensorPin);
      analogReading = analogRead(sensorPin);
      while(analogReading < cuePressure){
        pointerPos -= 1;
        pinkyPos -= 1;
        pointer.write(pointerPos);
        pinky.write(pinkyPos);
        analogReading = analogRead(sensorPin);
        Serial.println(analogReading);
        delay(20);
      }
    }

    // Wrist Control (wrist actuator is not a servo!)
    else if (GamePad.isTrianglePressed())
    {
      Serial.println("TRIANGLE");
      digitalWrite(wristPin1, HIGH);
      digitalWrite(wristPin2, LOW);
    }
    else if (GamePad.isCrossPressed())
    {
      Serial.println("CROSS");
      digitalWrite(wristPin1, LOW);
      digitalWrite(wristPin2, HIGH);
    }
    else
    {
      digitalWrite(wristPin1, LOW);
      digitalWrite(wristPin2, LOW);
    }
  }
}
