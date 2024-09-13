//Bluetooth Header file (/w the extension .h)
//The header file list the Bluetooth function declarations here in a class.
#ifndef BLUETOOTH_CONTROL_H
#define BLUETOOTH_CONTROL_H

#include <Arduino.h>

class BluetoothControl {
  public:
  BluetoothControl();
  void begin();
  void checkCommand();
  static bool reqTempAccess();
  static bool tempAccess;

  private:
  String readString;

  int ledRed =      A0;
  int ledGreen =    A1;
  int solenoidPin = A3;
};

#endif
