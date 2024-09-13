//RFID Source file (w/ extension .cpp)
//The source file contains the actual code for the RFID module program.
#include "RFIDDoorLock.h"
#include "BluetoothControl.h"

#define RST_PIN      9
#define SS_PIN      10
#define ledRed      A0
#define ledGreen    A1
#define buzzerPin   A2
#define solenoidPin A3

MFRC522 mfrc522(SS_PIN, RST_PIN);

int RFIDDoorLock::scanCount = 0;

RFIDDoorLock::RFIDDoorLock() {}

void RFIDDoorLock::setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(solenoidPin, OUTPUT);
  
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledGreen, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(solenoidPin, LOW);
}

void RFIDDoorLock::loop() {
  // Check if a new card is present OR read the Tag serial no.
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(25);
    return;
  }

  // The loop runs a number of times equal to the mfrc522.uid.size
  // Appends 0 to single-digit Hexa no. to ensure all bytes represent in two-digit Hexa no.
  // Construct the UID by converting each byte into Hexadecimal
  String rfidTag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfidTag += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
    rfidTag += String(mfrc522.uid.uidByte[i], HEX);
    rfidTag.toUpperCase();
  }
  
  Serial.println("\nRFID Tag Detected:" + rfidTag);
  
  // Tag UID comparison with the predefined UID
  if (rfidTag == validTag) {
    Serial.println("Access Granted!");
    unlockDoor();
  } else if (BluetoothControl::reqTempAccess()) {
    checkTempAccess();
  } else {
    Serial.println("Access denied!");
    scanCount++;
    if (scanCount == 3) {
      activateBuzzer();
    }
  }
  
  mfrc522.PICC_HaltA();  //Halt the RFID module operation and prepare for the next scan
}

void RFIDDoorLock::unlockDoor() {
  Serial.println("Unlocking door...");
  
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, HIGH);
  digitalWrite(solenoidPin, HIGH);  // Energise solenoid to unlock
  delay(3000);                      // Keep the solenoid energised for 3 seconds

  digitalWrite(ledRed, HIGH);
  digitalWrite(ledGreen, LOW);
  digitalWrite(solenoidPin, LOW);   // De-energise the solenoid to lock
  
  Serial.println("Door locked.");
  scanCount = 0;  //Reset the scanCOunt after successful match
}

void RFIDDoorLock::activateBuzzer() {
  Serial.println("Too many attempts!");
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

  scanCount = 0;  //Reset the scanCount
}

void RFIDDoorLock::checkTempAccess() {
  String rfidTag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfidTag += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
    rfidTag += String(mfrc522.uid.uidByte[i], HEX);
    rfidTag.toUpperCase();
  }
  
  Serial.println("\nRFID Tag Detected:" + rfidTag);
  
  if (rfidTag == tempTag) {
    unlockDoor();
  } else {
    Serial.println("Access Denied!");
    scanCount++;
    if (scanCount == 3) {
      activateBuzzer();
    }
  }
  mfrc522.PICC_HaltA();  //Halt the RFID module and prepare for the next scan
}
