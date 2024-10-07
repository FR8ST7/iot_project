#include <DHT.h>
#include <DHT_U.h>

// Define the DHT11 sensor
#define DHTPIN 2       // Pin connected to the DHT11 data pin
#define DHTTYPE DHT11  // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Define the LED pin
#define LEDPIN 8

// Define the temperature and humidity thresholds
const float tempThreshold = 30.0;  // Example: 30°C
const float humidityThreshold = 70.0;  // Example: 70% humidity

void setup() {
  // Initialize the DHT11 sensor
  dht.begin();

  // Set the LED pin as an output
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);  // Ensure LED is off initially

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read temperature as Celsius
  float tempC = dht.readTemperature();
  // Read humidity
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(tempC) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print temperature and humidity values to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Turn the LED on if temperature or humidity exceeds the thresholds
  if (tempC > tempThreshold || humidity > humidityThreshold) {
    digitalWrite(LEDPIN, HIGH);  // Turn on LED
    Serial.println("LED ON");
  } else {
    digitalWrite(LEDPIN, LOW);   // Turn off LED
    Serial.println("LED OFF");
  }

  // Wait a few seconds before the next reading
  delay(2000);
}
