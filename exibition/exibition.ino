#define BLYNK_TEMPLATE_ID "TMPL3k1WYGek_"  // Replace with your actual template ID
#define BLYNK_TEMPLATE_NAME "FOODTECH"     // Replace with your actual template name
#define BLYNK_AUTH_TOKEN "On1_TNajtQ1vbojyl_d-KcSaU-4oBNSQ"  // Replace with your actual auth token

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>  // Include the MQTT library

// Wi-Fi credentials
const char* ssid = "HOTSUPPORT";
const char* password = "Emilia@143";

// ThingSpeak API credentials
unsigned long myChannelNumber = 2686327;
const char* myWriteAPIKey = "YQ2MF5O3INY51I2D";

// MQTT broker details
const char* mqttServer = "test.mosquitto.org";  // Use a public MQTT broker
const int mqttPort = 1883;
const char* mqttTopic = "esp8266";  // Topic to publish food status

// DHT11 Pin and Type
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


int mq2DigitalPin = D1;
int mq135DigitalPin = D2;  
int buzzerPin = D4;  // Buzzer pin

WiFiClient espClient;
PubSubClient mqttClient(espClient);  
WiFiClient client;  

String previousFoodStatus = "";  

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(mq2DigitalPin, INPUT);
  pinMode(mq135DigitalPin, INPUT);
  pinMode(buzzerPin, OUTPUT);  
  digitalWrite(buzzerPin, LOW);  

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  
  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Setup MQTT client
  mqttClient.setServer(mqttServer, mqttPort);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void loop() {
  Blynk.run();
  mqttClient.loop();  // Maintain MQTT connection

  // Reading MQ2 analog value and converting to gas concentration
  int mq2AnalogValue = analogRead(A0);
  float mq2GasPPM = 1000 * (mq2AnalogValue / 1023.0);
  
  // Reading MQ135 analog value and converting to gas concentration
  int mq135AnalogValue = analogRead(A0);
  float mq135GasPPM = 1000 * (mq135AnalogValue / 1023.0);

  // Breakdown MQ2 readings into individual gases
  float methanePPM = mq2GasPPM * 0.40;
  float lpgPPM = mq2GasPPM * 0.30;

  // Breakdown MQ135 readings into individual gases
  float co2PPM = mq135GasPPM * 0.40;
  float nh3PPM = mq135GasPPM * 0.20;

  // DHT11 readings
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println("=== Debugging Readings ===");
  Serial.print("Methane PPM: "); Serial.println(methanePPM);
  Serial.print("CO2 PPM: "); Serial.println(co2PPM);
  Serial.print("NH3 PPM: "); Serial.println(nh3PPM);
  Serial.print("Humidity: "); Serial.println(humidity);

  String foodStatus;
  
  
  bool isSpoiled = (methanePPM > 140.18 || co2PPM > 170.1 || nh3PPM > 90.3 || humidity > 50.5);

  if (isSpoiled) {
    foodStatus = "Spoiled";
    if (previousFoodStatus != "Spoiled") {
      Serial.println("Food is Spoiled - Activating Buzzer");
      digitalWrite(buzzerPin, HIGH);  
      delay(1000);  
      digitalWrite(buzzerPin, LOW);   
    }
  } else {
    foodStatus = "Fresh";
    Serial.println("Food is Fresh - Buzzer Off");
    digitalWrite(buzzerPin, LOW);   // Ensure the buzzer is off
  }

  // Display all readings on Serial Monitor
  Serial.println("=== Gas Sensor Readings ===");
  Serial.print("MQ2 - Methane (CH4): "); Serial.print(methanePPM); Serial.println(" ppm");
  Serial.print("MQ2 - LPG: "); Serial.print(lpgPPM); Serial.println(" ppm");

  Serial.print("MQ135 - CO2: "); Serial.print(co2PPM); Serial.println(" ppm");
  Serial.print("MQ135 - NH3: "); Serial.print(nh3PPM); Serial.println(" ppm");

  Serial.println("=== DHT11 Sensor Readings ===");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" *C");

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
  if (responseCode == 200) {
    Serial.println("Data successfully sent to ThingSpeak");
  } else {
    Serial.println("Problem sending data to ThingSpeak. HTTP error code: " + String(responseCode));
  }

  // Send data to Blynk
  Blynk.virtualWrite(V1, methanePPM);
  Blynk.virtualWrite(V2, lpgPPM);
  Blynk.virtualWrite(V3, co2PPM);
  Blynk.virtualWrite(V4, nh3PPM);
  Blynk.virtualWrite(V5, temperature);
  Blynk.virtualWrite(V6, humidity);

  // Publish food status to MQTT topic
  if (foodStatus != previousFoodStatus) {
    String mqttMessage = "Food status: " + foodStatus;
    mqttClient.publish(mqttTopic, mqttMessage.c_str());
    Serial.println("Published to MQTT: " + mqttMessage);
    previousFoodStatus = foodStatus;  // Update previous status
  }

  delay(20000); 
}
