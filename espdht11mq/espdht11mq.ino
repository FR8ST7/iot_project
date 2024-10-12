#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN D4         // Pin for DHT11
#define DHTTYPE DHT11     // DHT11 Sensor Type
DHT dht(DHTPIN, DHTTYPE);

const int mq135AnalogPin = A0;  // Analog output pin for MQ135
const int mq135DigitalPin = D3; // Digital output pin for MQ135

const char* ssid = "HOTSUPPORT";           // Replace with your WiFi SSID
const char* password = "Emilia@143";   // Replace with your WiFi password

const char* apiKey = "your_thingspeak_api_key";  // ThingSpeak API Key
const char* server = "api.thingspeak.com";

WiFiClient client;

unsigned long myChannelNumber = 2686327;  // Replace with your channel number
const char* myWriteAPIKey = "G9SSGDW83GLDGCR8";  // Replace with your ThingSpeak Write API Key

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Initialize DHT11 sensor
  dht.begin();

  // Initialize MQ135 digital pin
  pinMode(mq135DigitalPin, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read temperature and humidity from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read gas concentration from MQ135
  int gasAnalogValue = analogRead(mq135AnalogPin);   // Analog reading
  int gasDigitalValue = digitalRead(mq135DigitalPin); // Digital reading (threshold)

  // Debug prints
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Gas (Analog): ");
  Serial.print(gasAnalogValue);
  Serial.print(", Gas (Digital): ");
  Serial.println(gasDigitalValue);

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, gasAnalogValue);     // Analog value from MQ135
  ThingSpeak.setField(4, gasDigitalValue);    // Digital value from MQ135

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.println("Failed to send data to ThingSpeak");
  }

  delay(20000);  // Wait 20 seconds before next update
}
