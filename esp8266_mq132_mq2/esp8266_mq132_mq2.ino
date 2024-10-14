#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>  // Include ThingSpeak library

// DHT11 Setup
#define DHTPIN D3  // DHT11 data pin connected to GPIO 0 (D3)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);  // Create DHT object

// Pin definitions for MQ2 and MQ135
int mq2DigitalPin = D1;  // MQ2 digital output
int mq135DigitalPin = D2;  // MQ135 digital output

// WiFi credentials (replace with your actual WiFi network credentials)
const char* ssid = "HOTSUPPORT";
const char* password = "Emilia@143";

// ThingSpeak settings
unsigned long myChannelNumber = 2686327;  // Replace with your ThingSpeak Channel Number
const char* myWriteAPIKey = "G9SSGDW83GLDGCR8";     // Replace with your ThingSpeak Write API Key

WiFiClient client;  // Initialize WiFi client

void setup() {
  // Start serial communication
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read DHT11 temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Read MQ2 analog value
  Serial.println("Reading MQ2...");
  int mq2AnalogValue = analogRead(A0);  // Read from A0 for MQ2
  float mq2GasPPM = 1000 * (mq2AnalogValue / 1023.0);  // Convert to gas concentration

  // Break down MQ2 readings into individual gases (rough percentages)
  float methanePPM = mq2GasPPM * 0.40;
  float lpgPPM = mq2GasPPM * 0.30;
  float hydrogenPPM = mq2GasPPM * 0.10;
  float coPPM = mq2GasPPM * 0.10;
  float smokePPM = mq2GasPPM * 0.10;

  // Read MQ135 analog value
  Serial.println("Reading MQ135...");
  int mq135AnalogValue = analogRead(A0);  // Read from A0 for MQ135
  float mq135GasPPM = 1000 * (mq135AnalogValue / 1023.0);  // Convert to gas concentration

  // Break down MQ135 readings into individual gases (rough percentages)
  float co2PPM = mq135GasPPM * 0.40;
  float nh3PPM = mq135GasPPM * 0.20;
  float noxPPM = mq135GasPPM * 0.15;
  float alcoholPPM = mq135GasPPM * 0.15;
  float benzenePPM = mq135GasPPM * 0.10;

  // Display sensor data (including original outputs)
  Serial.print("MQ2 - Methane (CH4): ");
  Serial.print(methanePPM);
  Serial.print(" ppm, LPG: ");
  Serial.print(lpgPPM);
  Serial.print(" ppm, Hydrogen (H2): ");
  Serial.print(hydrogenPPM);
  Serial.print(" ppm, CO: ");
  Serial.print(coPPM);
  Serial.print(" ppm, Smoke: ");
  Serial.print(smokePPM);
  Serial.println(" ppm");

  Serial.print("MQ135 - CO2: ");
  Serial.print(co2PPM);
  Serial.print(" ppm, NH3: ");
  Serial.print(nh3PPM);
  Serial.print(" ppm, NOx: ");
  Serial.print(noxPPM);
  Serial.print(" ppm, Alcohol: ");
  Serial.print(alcoholPPM);
  Serial.print(" ppm, Benzene: ");
  Serial.print(benzenePPM);
  Serial.println(" ppm");

  // Determine food freshness based on gas concentration thresholds
  String foodStatus = "Fresh";
  int foodIsSpoiled = 0;  // 0 means fresh, 1 means spoiled

  if (methanePPM > 300 || co2PPM > 500 || nh3PPM > 200) {
    foodStatus = "Spoiled";
    foodIsSpoiled = 1;  // Spoiled
  }

  // Display food freshness status
  Serial.print("Food Status: ");
  Serial.println(foodStatus);

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, mq2GasPPM);      // Total gas PPM from MQ2
  ThingSpeak.setField(4, mq135GasPPM);    // Total gas PPM from MQ135
  ThingSpeak.setField(5, foodIsSpoiled);  // Food status (1 = Spoiled, 0 = Fresh)

  // Write data to ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.println("Failed to send data to ThingSpeak. Error code: " + String(x));
  }

  // Wait 20 seconds between updates (ThingSpeak limit)
  delay(20000);
}
