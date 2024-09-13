#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN      9
#define SS_PIN      10
#define ledRed      A0
#define ledGreen    A1
#define buzzerPin   A2
#define solenoidPin A3

MFRC522 mfrc522(SS_PIN, RST_PIN);

const String validTag = " 73 47 1D 95";  //This is case sensitive, mind the letters and space!
const int maxAttempt = 3;
unsigned long scanCount = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();  //Begins the Serial Peripheral Interface communication
  mfrc522.PCD_Init();
  Serial.println("\nScan RIFD tag/card to unlock the door.");
  
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  
  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);  //Ensure solenoid is initially locked
}

void loop() {
  // Check if a new card is present OR read the Tag serial no.
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  // The loop runs number of times equal to the mfrc522.uid.size
  // Appends 0 to single-digit Hexa no. to ensure all bytes represent in two-digit Hexa no.
  // Construct the UID by converting each byte into Hexadecimal
  String rfidTag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfidTag += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
    rfidTag += String(mfrc522.uid.uidByte[i], HEX);
    rfidTag.toUpperCase();
  }

    Serial.println("\nRFID Tag Detected:" + rfidTag);
  
  // Tag UID comaprison with the predefined UID
  if (rfidTag == validTag) {
    Serial.println("Access Granted");
    unlockDoor();
  } else {
    Serial.println("Access denied!");
    scanCount++;
    delay(1000);
    if (scanCount >= maxAttempt) {
      activateBuzzer();
    }
  }
  
  mfrc522.PICC_HaltA();  //Halt the RFID module operation and prepare for the next scan
}

void unlockDoor() {
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

void activateBuzzer() {
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

  scanCount = 0;  // Reset the scanCount
}
