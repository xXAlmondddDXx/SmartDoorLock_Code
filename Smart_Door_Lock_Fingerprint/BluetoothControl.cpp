//Bluetooth Source file (w/ extension .cpp)
//The source file contains the actual code for the Bluetooth module program.
//Replace your door access command under [if readString == ""].
#include "BluetoothControl.h"

bool BluetoothControl::tempAccess = false;  // Add this global variable

BluetoothControl::BluetoothControl() {  //The function is part of the BluetoothControl class
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(solenoidPin, OUTPUT);
}

 void BluetoothControl::begin() {  //Set rate for communicating with phone
    Serial.begin(9600);
 }

void BluetoothControl::checkCommand() {
  while(Serial.available()){    //Check for available byte to read
    delay (10);                 //Delay to stabilise the reading
    char c = Serial.read();     //Conduct a serial read
    if(c == '#') {
      break;                    //Stop reading when '#' is encountered
    }
    readString += c;            //Appends c, (readString = readString + c)
  }

    if(readString.length() > 0) {
      readString.toLowerCase();
      Serial.println("\nReceived " + readString);   //Feedback to serial monitor

      if(readString == "open sesame") {
        Serial.println("Door is unlocked");  //Feedback to serial monitor
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(solenoidPin, HIGH);
        delay(3000);
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(solenoidPin, LOW);
    } else if (readString == "temporary access") {
      Serial.println("Visitor portal opened");
      tempAccess = true;
    } else if (readString == "close temporary access") {
      Serial.println("Visitor portal closed");
      tempAccess = false;
    } else {
        Serial.println("Invalid command: " + readString);  //Invalid command
    }
    readString = "";  //Clear readString for the next data
  }
}

bool BluetoothControl::reqTempAccess() {
  return tempAccess;
}
