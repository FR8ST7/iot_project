void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED pin
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on
  delay(1000);                     // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
  delay(1000);                     // Wait for a second
}
