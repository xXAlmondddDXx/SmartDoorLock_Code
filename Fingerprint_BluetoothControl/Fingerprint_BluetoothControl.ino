String readString;
#define ledRed      A0
#define ledGreen    A1
#define solenoidPin A3

void setup()
{
  Serial.begin(9600);         //Set rate for communicating with phone
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(solenoidPin, OUTPUT);

  digitalWrite(ledRed, HIGH);

  Serial.println("\nWaiting for Bluetooth command...");
}

void loop()
{
  while (Serial.available())   //Check if there are available bytes to read
  {
    delay(10);                 //Delay to stabilise the reading
    char c = Serial.read();    //Conduct a serial read
    if (c == '#'){
      break;                   //Stop reading when '#' is encountered
    }
    readString += c;           //Means readString = readString + c
  }

    if (readString.length() >0) {
      readString.toLowerCase();  //Convert the command to lowercase

      Serial.println("\nReceived: " + readString);  //Feedback to serial monitor
      if(readString == "open sesame") {
        Serial.println("Door is unlocked");  //Feedback to serial monitor
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(solenoidPin, HIGH);
      } else if (readString == "close sesame") {
        Serial.println("Door is locked");  //Feedback to serial monitor
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(solenoidPin, LOW);
      } else if (readString == "temporary access") {
        Serial.println("Visitor Portal Opened!");
      } else if (readString == "close temporary access") {
        Serial.println("Visitor Portal Closed!");
      } else {
        Serial.println("Invalid command: " + readString);   //Feedback to serial monitor for invalid command
      }
      readString="";  //Clear readString for the next data
    }
}
