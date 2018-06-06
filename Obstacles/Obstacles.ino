#include <Servo.h>
Servo servo;

//Ultrasound sensor
#define trig_pin 11
#define echo_pin 12

//Servo Motor
#define servoPin 10

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(servoPin, OUTPUT);

  servo.attach(servoPin);
  servo.write(90);
  delay(1000);
  servo.detach();
}

//void loop() {
//  servo.write(90);
//  delay(1000);
//  servo.detach();
//  // put your main code here, to run repeatedly:
//  if (measureDistance()<20.0){
//    servo.attach(servoPin);
//    servo.write(180);
//    delay(5000);
//  }
//}

void loop(){
  if(measureDistance() < 20.0){
    servo.attach(servoPin);
    servo.write(180);
    delay(3000);
    servo.write(90);
    delay(10000);
    servo.detach();
  }
  delay(100);
}
float measureDistance(){
  float timeOfFlight, distanceCm;
  float c_s = 0.0343; //Speed of sound in cm/microseconds
  sendSoundPulse(); //Module sends out 8 pulses, and sets echo_pin to high. When echo is detected, echo_pin is set back to low
  timeOfFlight = pulseIn(echo_pin, HIGH); //Duration that echo_pin is high in microseconds.
  distanceCm = timeOfFlight * c_s / 2.;
  Serial.println(distanceCm);
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
