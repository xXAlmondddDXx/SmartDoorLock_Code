/***************************************************************************
Author: Chen Yu Wei (T00228555)
Subject: Capstone Project
Title: Arduino Smart Door Lock System
Programme: Level 8 Bachelor Sci in Manufacturing Engineering
Institution: Munster Technological University (Kerry)

Original by Limor Fried/Ladyada for Adafruit Industries
Modified by Chen Yu Wei, For Final Year Capstone Project

https://www.youtube.com/channel/UCM6rbuieQBBLFsxszWA85AQ?sub_confirmation=1
***************************************************************************/

// For UNO and others without hardware serial, we must use software serial
// Pin #2 is IN from sensor (TXD)
// Pin #3 is OUT from arduino (RXD))

// For Bluetooth module replace your access command at .cpp file under [if readString == ""].
// For RFID module replace your card/tag UID at .h file under [validTag = ""].
// For Ultrasonic sensor replace your sensing distance at .cpp file under [float distance = (duration*sp of sound)/2].

//Include library
#include <Adafruit_Fingerprint.h>
#include "BluetoothControl.h"
#include "RFIDDoorLock.h"
#include "UltrasonicSensor.h"

//Declare variable pins
#define ledRed      A0
#define ledGreen    A1
#define buzzerPin   A2
#define solenoidPin A3
#define echoPin     4
#define trigPin     5

//Variables that count acrros the function call
const unsigned long doorOpenAllowance = 60000;  // 1 minute, replace the door opening allowance time here in milliseconds
const int maxFingerAttempt = 3;                 // replace your max fingerprint attempt here

unsigned long doorOpenTime = 0;
unsigned long fingerAttempt = 0;

//Software Serial
SoftwareSerial fingerprintSerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);
BluetoothControl btControl;
RFIDDoorLock rfidF;
UltrasonicSensor ultraSensor(trigPin, echoPin);


void setup()  //Initialisation of the components such as input or output
{
  // Call RFID setup function
  rfidF.setup();

  Serial.begin(9600);   // Initialise components at 9600 baud rate
  
  btControl.begin();    // Initialise the Bluetooth functions
  
  ultraSensor.begin();  // Initialise the Ultrasonic functions
  
  //Set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode( 4, INPUT);
  pinMode( 5, OUTPUT);

  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

  while (!Serial);    //For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\nAdafruit finger detect test");
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }  //Put a small delay to stabilise the program
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  
  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  Serial.println("OR\nAcquiring access command from the user...");
  Serial.println("OR\nScan RFID tag/card to unlock...");
  Serial.println("\nCounting the duration of the door being opened...");
}


void loop()  // Code run over and over again
{
  //Call for Bluetooth loop function
  btControl.checkCommand();


  //Call for RFID loop function
  rfidF.loop();


  //Call for Ultrasonic Sensor loop function
  float distance = ultraSensor.getDistance();
  checkDoorOpenAlert(distance);


  //Call for Fingerprint function
  getFingerprintIDez();
  delay(25);  //don't need to run this at full speed to ensure the stability of system
}

void checkDoorOpenAlert(float distance) {
  if (distance >= 10 && (millis() - doorOpenTime >= doorOpenAllowance)) {
    //Alert for open door
    Serial.println("\nPlease shut the door!");
    doorAlarm();
    doorOpenTime = millis();  //Reset the timer
  } else if (distance <= 10 || (millis() - doorOpenTime >= doorOpenAllowance)) {
    doorOpenTime = millis();  //Reset the timer
  }
}

void doorAlarm() {
  digitalWrite(ledRed, LOW);
  noTone(buzzerPin);

  unsigned long endTime = millis() + 5000;  //Set the time as current
  while (millis() < endTime) {              //Run the buzzer for 5s
    digitalWrite(ledRed, HIGH);
    tone(buzzerPin, 500);
    delay(250);
    digitalWrite(ledRed, LOW);
    noTone(buzzerPin);
    delay(250);
  }
  noTone(buzzerPin);
  digitalWrite(ledRed, HIGH);
}

uint8_t getFingerprintID()  //unsigned integer of length 8 bits
{
  uint8_t p = finger.getImage();
  switch (p)  //select one of many code blocks to be executed
  {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:  //specify some code to run if no case match
      Serial.println("Unknown error");
      return p;
  }

  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
    p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  return finger.fingerID;
}

// Returns error message if failed, otherwise returns ID#
// Fingerprint attempt functions
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK){
  return -1;
}

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK){
  return -1;
}

  //Mateching the fingerprint...
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK){
    Serial.println("\nAccess Denied!");
    fingerAttempt++;
    if (fingerAttempt >= maxFingerAttempt) {
      Serial.println("Too many attemps!");
      digitalWrite(ledRed, LOW);
      noTone(buzzerPin);
  
      unsigned long endTime = millis() + 5000;
      while (millis() < endTime) {
        digitalWrite(ledRed, HIGH);
        tone(buzzerPin, 500);
        delay(250);
        digitalWrite(ledRed, LOW);
        noTone(buzzerPin);
        delay(250);
      }
  
      noTone(buzzerPin);
      digitalWrite(ledRed, HIGH);

      fingerAttempt = 0;
      doorOpenTime = millis();
    }
    return -1;
  }

  fingerprintMatch();

  return finger.fingerID;
}

void fingerprintMatch() {
  //found a match!
  Serial.print("\nFound ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  Serial.println("Access Granted!");
  
  //Additional actions after successful fingerprint match
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, HIGH);
  digitalWrite(solenoidPin, HIGH);
  delay(3000);
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledGreen, LOW);
  digitalWrite(solenoidPin, LOW);

  //Reset fingerCount after successful match
  fingerAttempt = 0;
  doorOpenTime = millis();
}
