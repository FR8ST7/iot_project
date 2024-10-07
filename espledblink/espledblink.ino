const int ledPin = 5;  // GPIO pin where the LED is connected (D1 in NodeMCU)

void setup() {
  pinMode(ledPin, OUTPUT);  // Initialize the GPIO pin as an output
}

void loop() {
  digitalWrite(ledPin, HIGH);  // Turn the LED on
  delay(1000);                 // Wait for a second
  digitalWrite(ledPin, LOW);   // Turn the LED off
  delay(1000);                 // Wait for a second
}
