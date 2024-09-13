//RFID Header file (/w the extension .h)
//The header file list the RFID function declarations here in a class.
//Replace the RFID UID for the validTag inside the "".
#ifndef RFIDDoorLock_H
#define RFIDDoorLock_H

#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>

class RFIDDoorLock {
  public:
    RFIDDoorLock();
    void setup();
    void loop();
    void unlockDoor();
    void activateBuzzer();
    void checkTempAccess();
  
  private:
    static int scanCount;
    const String validTag = " 73 47 1D 95";  //The UID is case senstivie, mind the letter and the space
    const String tempTag  = " 23 80 85 90";
};

#endif
