//Ultrasonic Source file (w/ extension .cpp)
//The source file contains the actual code for the Ultrasonic sensor program.
//Replace your sensing distance under [float distance = (duration*sp of sound)/2].
#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin) {
  _trigPin = trigPin;  //underscore use to clarify private variables
  _echoPin = echoPin;
}

void UltrasonicSensor::begin() {
  pinMode(_echoPin, INPUT);     //Set the echo pin as input to receive sound wave
  pinMode(_trigPin, OUTPUT);    //Set the trigger pin as output to transmit sound wave
}

float UltrasonicSensor::getDistance() {
  //The trigger point sends out 8 cycle sonic burst from the transmitter
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);

  //Read the echoPin to acquire the duration of the ultrasonic pulse to return
  //Convert into distance via speed of sound @ 0.0343 cm/µs
  float duration = pulseIn(_echoPin, HIGH);
  float distance = (duration*0.0343)/2;

  return distance;
}
