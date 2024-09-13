const int echoPin = 4;  // Trig pin of HC-SR04 connected to Arduino pin 9
const int trigPin = 5;  // Echo pin of HC-SR04 connected to Arduino pin 10
#define buzzerPin A2
#define ledRed A0

float duration, distance;
const unsigned long doorOpenAllowance = 60000;
unsigned long doorOpenTime = 0;

void setup() {
  Serial.begin(9600);   // Initialize serial communication at 9600 baud rate
  pinMode(4, INPUT);    // Set the trigPin as an OUTPUT
  pinMode(5, OUTPUT);   // Set the echoPin as an INPUT
  pinMode(A0, OUTPUT);
  pinMode(A2, OUTPUT);

  digitalWrite(A0, HIGH);

  Serial.println("\nCounting the duration of door being opened...");
}

void loop() {
  digitalWrite(trigPin, LOW);  // Clear the trigPin
  delayMicroseconds(2);

  // Trigger the sensor by setting the trigPin on HIGH state for 10 microseconds
  // Sends out 8 cycle sonic burst from the transmitter
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, which will give us the time taken for the ultrasonic pulse to return
  duration = pulseIn(echoPin, HIGH);
  delay(100);

  // Calculate the distance (in centimeters per microseconds) based on the speed of sound (343m/s)
  // and the time taken for the pulse to return
  distance = (duration * 0.0343)/ 2;
  Serial.print("\nDistance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance >= 10 && (millis() - doorOpenTime >= doorOpenAllowance)) {
    //Alert for open door
    Serial.println("Please shut the door!");
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
