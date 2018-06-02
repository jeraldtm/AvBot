#include <Servo.h>
Servo servo;

//Ultrasound sensor
#define trig_pin 11
#define echo_pin 12

//Motors
#define motorA_pin 9
#define motorB_pin 4
#define motorC_pin 8
#define motorD_pin 7
#define powerLeft_pin 5
#define powerRight_pin 6

//Speed IR Sensors
#define leftSensor_pin 2
#define rightSensor_pin 3

//Servo Motor
#define servoPin 10

//Variables
int powerMax = 130;
int powerLow = 70;
int steps = 50;
int leftSteps = 0;
int rightSteps = 0;
float numberOfMeasurements = 5.;
int rampAdjust = 5;

//Delays
int stopDelay = 1000;
int reverseDelay = 500;
int measureDelay = 100;
int slightTurnDelay = 200;
int halfTurnDelay = 400;
int fullTurnDelay = 500;
int servoDelay = 200;

//Distances
float crashDistance = 10.;
float sideDistance = 20.;
float stopDistance = 15.;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(leftSensor_pin, INPUT);
  pinMode(rightSensor_pin, INPUT);
  pinMode(motorA_pin, OUTPUT);
  pinMode(motorB_pin, OUTPUT);
  pinMode(motorC_pin, OUTPUT);
  pinMode(motorD_pin, OUTPUT);
  pinMode(servoPin, OUTPUT);
  attachInterrupt(0, countLeft, RISING);
  attachInterrupt(1, countRight, RISING);
}

void loop() {
  moveCar(steps);
  stopCar();
  delay(stopDelay);
  measureSurroundings();
}

void moveCar(int steps){
  leftSteps = 0;
  rightSteps = 0;
  rampDown(powerMax);
  while ((leftSteps < steps) and (rightSteps < steps)){
    if (measureDistance() > stopDistance){
      forward(powerMax);
    }
  }

  Serial.print("Left: ");
  Serial.println(leftSteps);
  Serial.print("Right: ");
  Serial.println(rightSteps);

  while (leftSteps < rightSteps){
    if (measureDistance() > stopDistance){
      analogWrite(powerRight_pin, 0);
      analogWrite(powerLeft_pin, powerLow);
    }
  }

  while (rightSteps < leftSteps){
    if (measureDistance() > stopDistance){
      analogWrite(powerLeft_pin, 0);
      analogWrite(powerRight_pin, powerLow);
    }
  }

  Serial.print("Final Left: ");
  Serial.println(leftSteps);
  Serial.print("Final Right: ");
  Serial.println(rightSteps);
  
  leftSteps = 0;
  rightSteps = 0;
}

void measureSurroundings(){
  servo.attach(servoPin);
  long leftDistance = 0;
  long rightDistance = 0;
  long centreDistance = 0;

  servo.write(0);
  delay(servoDelay);
  for (int x = 0; x < numberOfMeasurements; x++) {
    rightDistance += measureDistance();
    delay(measureDelay);
  }
  rightDistance = rightDistance/(numberOfMeasurements);

  servo.write(70);
  delay(servoDelay);
  for (int x = 0; x < numberOfMeasurements; x++) {
    centreDistance += measureDistance();
    delay(measureDelay);
  }
  centreDistance = centreDistance/(numberOfMeasurements);

  servo.write(160);
  delay(servoDelay);
  for (int x = 0; x < numberOfMeasurements; x++) {
    leftDistance += measureDistance();
    delay(measureDelay);
  }
  leftDistance = leftDistance/(numberOfMeasurements);
  servo.write(70);
  
  delay(measureDelay);
  delay(servoDelay);
  servo.detach();

  Serial.print("Left Dist: ");
  Serial.println(leftDistance);
  Serial.print("Centre Dist: ");
  Serial.println(centreDistance);
  Serial.print("Right Dist: ");
  Serial.println(rightDistance);
  
  if (centreDistance < crashDistance){
    if ((leftDistance > crashDistance) or (rightDistance > crashDistance)){
      if (leftDistance > rightDistance){
        Serial.println("Turn Left");
        turnLeft(powerMax); //turn left
        delay(fullTurnDelay);
        stopCar();
        delay(stopDelay);
      } else {
        Serial.println("Turn Right");
        turnRight(powerMax); //turn right
        delay(fullTurnDelay);
        stopCar();
        delay(stopDelay);
      }
    } else{
      Serial.println("U turn");
      turnLeft(powerMax); //U turn
      delay(fullTurnDelay);
      stopCar();
      delay(stopDelay);
      turnLeft(powerMax);
      delay(fullTurnDelay);
      stopCar();
      delay(stopDelay);
    }
  }
  
  else if ((leftDistance < sideDistance) and (rightDistance > sideDistance)){
    if (leftDistance < crashDistance){
      Serial.println("Reverse and Right");
      backwards(powerMax); //reverse and right
      delay(reverseDelay);
      stopCar();
      delay(stopDelay);
      turnRight(powerMax);
      delay(halfTurnDelay);
      stopCar();
      delay(stopDelay);
    }
    else{
      Serial.println("Slight Right");
      turnRight(powerMax); //slight right
      delay(slightTurnDelay);
      stopCar();
      delay(stopDelay);
    }
  }

  else if ((rightDistance < sideDistance) and (leftDistance > sideDistance)){
    if (rightDistance < crashDistance){
      Serial.println("Reverse and Left");
      backwards(powerMax); //reverse and left
      delay(reverseDelay);
      stopCar();
      delay(stopDelay);
      turnLeft(powerMax);
      delay(halfTurnDelay);
      stopCar();
      delay(stopDelay);
    }
    else{
      Serial.println("Slight Left");
      turnLeft(powerMax); //slight left
      delay(slightTurnDelay);
      stopCar();
      delay(stopDelay);
    }
  }
}

float measureDistance(){
  float timeOfFlight, distanceCm;
  float c_s = 0.0343; //Speed of sound in cm/microseconds
  sendSoundPulse(); //Module sends out 8 pulses, and sets echo_pin to high. When echo is detected, echo_pin is set back to low
  timeOfFlight = pulseIn(echo_pin, HIGH); //Duration that echo_pin is high in microseconds.
  distanceCm = timeOfFlight * c_s / 2.;
//  Serial.println(distanceCm);
  return distanceCm;
}

void sendSoundPulse(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW); 
  //The first of the 8 echos reflect off a distant object, and is detected at the echo pin, after which the pin is set to low.
}

void forward(int power){
  digitalWrite(motorA_pin, HIGH);
  digitalWrite(motorB_pin, LOW);
  digitalWrite(motorC_pin, HIGH);
  digitalWrite(motorD_pin, LOW);
  analogWrite(powerLeft_pin, power);
  analogWrite(powerRight_pin, power);
}

void backwards(int power){
  digitalWrite(motorA_pin, LOW);
  digitalWrite(motorB_pin, HIGH);
  digitalWrite(motorC_pin, LOW);
  digitalWrite(motorD_pin, HIGH);
  analogWrite(powerLeft_pin, power);
  analogWrite(powerRight_pin, power);
}

void turnRight(int power){
  digitalWrite(motorA_pin, HIGH);
  digitalWrite(motorB_pin, LOW);
  digitalWrite(motorC_pin, LOW);
  digitalWrite(motorD_pin, HIGH);
  analogWrite(powerLeft_pin, power);
  analogWrite(powerRight_pin, power);
}

void turnLeft(int power){
  digitalWrite(motorA_pin, LOW);
  digitalWrite(motorB_pin, HIGH);
  digitalWrite(motorC_pin, HIGH);
  digitalWrite(motorD_pin, LOW);
  analogWrite(powerLeft_pin, power);
  analogWrite(powerRight_pin, power);
}

void stopCar(){
  digitalWrite(motorA_pin, LOW);
  digitalWrite(motorB_pin, LOW);
  digitalWrite(motorC_pin, LOW);
  digitalWrite(motorD_pin, LOW);
  analogWrite(powerLeft_pin, 0);
  analogWrite(powerRight_pin, 0);
}

void countLeft(){
  leftSteps++;
}

void countRight(){
  rightSteps++;
}

void rampDown(int power){
  int rampPower = 255;
  digitalWrite(motorA_pin, HIGH);
  digitalWrite(motorB_pin, LOW);
  digitalWrite(motorC_pin, HIGH);
  digitalWrite(motorD_pin, LOW);
  while (rampPower > power){
    rampPower -= rampAdjust;
    analogWrite(powerLeft_pin,rampPower);
    analogWrite(powerRight_pin,rampPower);
  }
}



