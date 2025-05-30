#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>  // Include the PubSubClient library for MQTT

// Define WiFi credentials
const char* ssid = "HOTSUPPORT";
const char* password = "Emilia@143";

// ThingSpeak API credentials
unsigned long myChannelNumber = 2686327;
const char* myWriteAPIKey = "H2ECAIHFVLT81JFU";

// MQTT Broker and topic details
const char* mqttServer = "broker.hivemq.com";  // Public MQTT broker (example: HiveMQ)
const int mqttPort = 1883;
const char* mqttTopic = "esp8266/sensors";     // MQTT topic to publish data

WiFiClient espClient;
PubSubClient mqttClient(espClient);  // Initialize MQTT client
DHT dht(D3, DHT11);  // DHT11 Pin and Type

// Pin definitions for MQ2 and MQ135
int mq2DigitalPin = D1;  // MQ2 digital output
int mq135DigitalPin = D2;  // MQ135 digital output

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(mq2DigitalPin, INPUT);
  pinMode(mq135DigitalPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(espClient);

  // Initialize MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  connectToMQTT();
}

// Function to connect to MQTT
void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP8266Client")) {  // Client ID
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(mqttClient.state());
      delay(2000);  // Wait before retrying
    }
  }
}

void loop() {
  // Ensure the MQTT connection is maintained
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();  // Keep the MQTT client alive

  // Reading MQ2 analog value and converting to gas concentration
  int mq2AnalogValue = analogRead(A0);
  float mq2GasPPM = 1000 * (mq2AnalogValue / 1023.0);
  
  // Reading MQ135 analog value and converting to gas concentration
  int mq135AnalogValue = analogRead(A0);
  float mq135GasPPM = 1000 * (mq135AnalogValue / 1023.0);

  // Break down MQ2 readings into individual gases (rough percentages)
  float methanePPM = mq2GasPPM * 0.40;
  float lpgPPM = mq2GasPPM * 0.30;
  float hydrogenPPM = mq2GasPPM * 0.10;
  float coPPM = mq2GasPPM * 0.10;
  float smokePPM = mq2GasPPM * 0.10;

  // Break down MQ135 readings into individual gases (rough percentages)
  float co2PPM = mq135GasPPM * 0.40;
  float nh3PPM = mq135GasPPM * 0.20;
  float noxPPM = mq135GasPPM * 0.15;
  float alcoholPPM = mq135GasPPM * 0.15;
  float benzenePPM = mq135GasPPM * 0.10;

  // DHT11 readings
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any readings failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Determine if food is fresh or spoiled based on gas concentrations
  String foodStatus;
  if (methanePPM > 200 || co2PPM > 400 || nh3PPM > 100) {
    foodStatus = "Spoiled";
  } else {
    foodStatus = "Fresh";
  }

  // Display all outputs on the Serial Monitor
  Serial.println("=== Gas Sensor Readings ===");
  Serial.print("MQ2 - Methane (CH4): "); Serial.print(methanePPM); Serial.println(" ppm");
  Serial.print("MQ2 - LPG: "); Serial.print(lpgPPM); Serial.println(" ppm");
  Serial.print("MQ2 - Hydrogen (H2): "); Serial.print(hydrogenPPM); Serial.println(" ppm");
  Serial.print("MQ2 - CO: "); Serial.print(coPPM); Serial.println(" ppm");
  Serial.print("MQ2 - Smoke: "); Serial.print(smokePPM); Serial.println(" ppm");

  Serial.print("MQ135 - CO2: "); Serial.print(co2PPM); Serial.println(" ppm");
  Serial.print("MQ135 - NH3: "); Serial.print(nh3PPM); Serial.println(" ppm");
  Serial.print("MQ135 - NOx: "); Serial.print(noxPPM); Serial.println(" ppm");
  Serial.print("MQ135 - Alcohol: "); Serial.print(alcoholPPM); Serial.println(" ppm");
  Serial.print("MQ135 - Benzene: "); Serial.print(benzenePPM); Serial.println(" ppm");

  Serial.println("=== DHT11 Sensor Readings ===");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" *C");

  // Display food status
  Serial.print("Food Status: ");
  Serial.println(foodStatus);

  // Update ThingSpeak fields with sensor data
  ThingSpeak.setField(1, methanePPM);
  ThingSpeak.setField(2, lpgPPM);
  ThingSpeak.setField(3, co2PPM);
  ThingSpeak.setField(4, nh3PPM);
  ThingSpeak.setField(5, temperature);

  // Write the data to ThingSpeak
  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  // Check if data was sent successfully to ThingSpeak
  if (responseCode == 200) {
    Serial.println("Data successfully sent to ThingSpeak");
  } else {
    Serial.println("Problem sending data to ThingSpeak. HTTP error code: " + String(responseCode));
  }

  // Create JSON payload for MQTT
  String payload = "{";
  payload += "\"methanePPM\":" + String(methanePPM) + ",";
  payload += "\"foodStatus\":\"" + foodStatus + "\"";  // Include only food status
  payload += "}";

  // Publish data to the MQTT broker
  if (mqttClient.publish(mqttTopic, payload.c_str())) {
    Serial.println("Data successfully sent to MQTT");
  } else {
    Serial.println("Failed to send data to MQTT");
  }

  delay(20000);  // Wait 20 seconds before the next loop iteration
}
