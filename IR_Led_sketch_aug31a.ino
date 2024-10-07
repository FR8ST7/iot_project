// Define the IR sensor and LED pins
#define IRSensorPin 2   // Pin connected to the IR sensor OUT pin
#define LEDPin 8        // Pin connected to the LED

void setup() {
  // Set the IR sensor pin as input
  pinMode(IRSensorPin, INPUT);

  // Set the LED pin as output
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);  // Ensure LED is off initially

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read the value from the IR sensor
  int sensorValue = digitalRead(IRSensorPin);

  // Check if the sensor detects an object
  if (sensorValue == LOW) {  // IR sensor output is LOW when an object is detected
    Serial.println("Object Detected!");
    digitalWrite(LEDPin, HIGH);  // Turn on the LED
  } else {
    Serial.println("No Object Detected.");
    digitalWrite(LEDPin, LOW);   // Turn off the LED
  }

  // Small delay before the next reading
  delay(100);
}
