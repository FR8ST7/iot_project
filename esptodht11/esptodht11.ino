#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN 2     // D4 pin on NodeMCU (GPIO2)
#define DHTTYPE DHT11   // Define DHT11 sensor

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "HOTSUPPORT";      // Replace with your Wi-Fi SSID
const char* password = "Emilia@143";  // Replace with your Wi-Fi Password
const char* apiKey = "IC25EWATN155R8TU";  // Replace with your ThingSpeak API key

WiFiClient client;

unsigned long myChannelNumber = 2686327;  // Replace with your ThingSpeak Channel Number

void setup() {
  Serial.begin(115200);
  delay(10);

  dht.begin();

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wi-Fi connected");

  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  // Reading temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // Read temperature in Celsius
  
  // Check if readings are valid
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  // Upload data to ThingSpeak
  ThingSpeak.setField(1, t);  // Temperature in Field 1
  ThingSpeak.setField(2, h);  // Humidity in Field 2
  
  int result = ThingSpeak.writeFields(myChannelNumber, apiKey);
  
  if (result == 200) {
    Serial.println("Data uploaded successfully");
  } else {
    Serial.println("Error uploading data: " + String(result));
  }

  // Wait 15 seconds before sending new data to ThingSpeak
  delay(15000);
}
