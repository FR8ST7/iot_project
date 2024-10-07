// Define the pins for MQ-2 sensor and alert
const int mq2AnalogPin = A0;  // Analog pin for MQ-2 sensor
const int mq2DigitalPin = 2;  // Digital pin for MQ-2 sensor
const int alertPin = 8;       // Pin for buzzer or LED

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set up the pins
  pinMode(mq2AnalogPin, INPUT);
  pinMode(mq2DigitalPin, INPUT);
  pinMode(alertPin, OUTPUT);
  digitalWrite(alertPin, LOW);  // Ensure the alert is off initially
}

void loop() {
  // Read the analog value from the MQ-2 sensor
  int sensorValue = analogRead(mq2AnalogPin);
  
  // Read the digital value from the MQ-2 sensor
  int digitalValue = digitalRead(mq2DigitalPin);
  
  // Convert the analog value to a voltage
  float voltage = sensorValue * (5.0 / 1023.0);

  // Print sensor values to the serial monitor
  Serial.print("Analog Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.print(" | Digital Sensor Value: ");
  Serial.println(digitalValue);

  // Methane detection using analog value (adjust threshold)
  if (sensorValue > 300) {  // Example threshold for methane detection
    Serial.println("Methane Detected (Analog)!");
    digitalWrite(alertPin, HIGH);  // Turn on the alert (buzzer/LED)
  } else {
    digitalWrite(alertPin, LOW);   // Turn off the alert (buzzer/LED)
  }

  // Methane detection using digital value
  if (digitalValue == HIGH) {  // Digital pin goes HIGH if gas detected
    Serial.println("Methane Detected (Digital)!");
    digitalWrite(alertPin, HIGH);  // Turn on the alert (buzzer/LED)
  }

  // Small delay before next reading
  delay(1000);
}
