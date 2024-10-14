#include <SoftwareSerial.h>

// Define the RX and TX pins for the Arduino
SoftwareSerial mySerial(10, 11);  // RX (D10), TX (D11)

int buzzerPin = 7;  // Pin for the buzzer
int irSensorPin = 8;  // Pin for the IR sensor
bool hasBeeped = false;  // Flag to check if the buzzer has beeped

void setup() {
  Serial.begin(115200);  // Initialize serial communication with the PC
  mySerial.begin(115200); // Initialize software serial communication with ESP8266
  pinMode(buzzerPin, OUTPUT);  // Set buzzer pin as output
  pinMode(irSensorPin, INPUT);  // Set IR sensor pin as input
  digitalWrite(buzzerPin, HIGH);  // Turn off the buzzer initially (active LOW)
}

void loop() {
  // Check if data is available from ESP8266
  if (mySerial.available() > 0) {
    String receivedData = mySerial.readStringUntil('\n');  // Read data until newline

    // Display the received data
    Serial.print("Received from ESP8266: ");
    Serial.println(receivedData);

    // Check the status of food and sound the buzzer if food is spoiled
    if (receivedData.indexOf("Spoiled") >= 0) {
      if (!hasBeeped) {  // Check if the buzzer has already beeped
        digitalWrite(buzzerPin, LOW);  // Turn on the buzzer (active LOW)
        Serial.println("Food is Spoiled! Buzzer ON!");
        delay(1000);  // Keep it on for 1 second
        digitalWrite(buzzerPin, HIGH);   // Turn off the buzzer
        hasBeeped = true;  // Set the flag to true to avoid multiple beeps
      }
    } else if (receivedData.indexOf("Fresh") >= 0) {
      digitalWrite(buzzerPin, HIGH);   // Turn off the buzzer (active LOW)
      Serial.println("Food is Fresh! Buzzer OFF.");
      hasBeeped = false;  // Reset the flag if food is fresh
    }
  }

  // Check if IR sensor detects an object (food)
  int irValue = digitalRead(irSensorPin);
  if (irValue == HIGH) {
    Serial.println("Food detected by IR sensor.");
  } else {
    Serial.println("No food detected by IR sensor.");
  }

  delay(1000);  // Delay for 1 second
}
