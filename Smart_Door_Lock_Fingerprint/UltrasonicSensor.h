//The Ultrasonic Header file (w/ extension .h)
//The header file list the Ulrasonic function declarations here in a class.
#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor {
  public:
  UltrasonicSensor(int trigPin, int echoPin);
  void begin();
  float getDistance();

  private:
  int _trigPin;
  int _echoPin;
};

#endif
