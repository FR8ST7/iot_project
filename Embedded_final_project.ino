#define BLYNK_TEMPLATE_ID "TMPL3k1WYGek_"  
#define BLYNK_TEMPLATE_NAME "FOODTECH"     
#define BLYNK_AUTH_TOKEN "On1_TNajtQ1vbojyl_d-KcSaU-4oBNSQ"

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Wi-Fi credentials
const char* ssid = "HOTSUPPORT";
const char* password = "Chahaein@143";

// ThingSpeak API credentials
unsigned long myChannelNumber = 2686327;
const char* myWriteAPIKey = "C792XZAEYRVEIF2D";

// MQTT broker details
const char* mqttServer = "test.mosquitto.org";  
const int mqttPort = 1883;
const char* mqttTopic = "esp8266";  

// DHT11 Pin and Type
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int mq2DigitalPin = D1;
int mq135DigitalPin = D2;  
int buzzerPin = D4;  

WiFiClient espClient;
PubSubClient mqttClient(espClient);  
WiFiClient client;  

String previousFoodStatus = "";  

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16 columns, 2 rows

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(mq2DigitalPin, INPUT);
  pinMode(mq135DigitalPin, INPUT);
  pinMode(buzzerPin, OUTPUT);  
  digitalWrite(buzzerPin, LOW);  

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");

  ThingSpeak.begin(client);  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

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

  // Initialize LCD
  Wire.begin(D6, D7);  // Set custom pins for SDA and SCL (D6 for SDA, D7 for SCL)
  lcd.begin(16, 2);     // Initialize LCD with 16 columns and 2 rows
  lcd.backlight();      // Turn on backlight
}

void loop() {
  Blynk.run();
  mqttClient.loop();  

  int mq2AnalogValue = analogRead(A0);
  float mq2GasPPM = 1000 * (mq2AnalogValue / 1023.0);

  int mq135AnalogValue = analogRead(A0);
  float mq135GasPPM = 1000 * (mq135AnalogValue / 1023.0);

  float methanePPM = mq2GasPPM * 0.40;
  float co2PPM = mq135GasPPM * 0.40;
  float nh3PPM = mq135GasPPM * 0.20;

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

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
  bool isSpoiled = (methanePPM > 300.18 || co2PPM > 170.1 || nh3PPM > 90.3 || humidity > 50.5);

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
    digitalWrite(buzzerPin, LOW);   
  }

  Serial.println("=== Gas Sensor Readings ===");
  Serial.print("MQ2 - Methane (CH4): "); Serial.print(methanePPM); Serial.println(" ppm");

  Serial.print("MQ135 - CO2: "); Serial.print(co2PPM); Serial.println(" ppm");
  Serial.print("MQ135 - NH3: "); Serial.print(nh3PPM); Serial.println(" ppm");

  Serial.println("=== DHT11 Sensor Readings ===");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" *C");

  Serial.print("Food Status: ");
  Serial.println(foodStatus);

  // Spoiled Ratio Calculation
  int spoiledRatio = 0;
  if (methanePPM > 140.18) spoiledRatio += 25;
  if (co2PPM > 170.1) spoiledRatio += 25;
  if (nh3PPM > 90.3) spoiledRatio += 25;
  if (humidity > 50.5) spoiledRatio += 25;

  // ThingSpeak Field Mapping
  ThingSpeak.setField(1, methanePPM);     // Methane
  ThingSpeak.setField(2, co2PPM);         // CO2
  ThingSpeak.setField(3, nh3PPM);         // NH3
  ThingSpeak.setField(4, temperature);    // Temp
  ThingSpeak.setField(5, humidity);       // Humidity
  ThingSpeak.setField(6, spoiledRatio);   // Spoiled %

  int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (responseCode == 200) {
    Serial.println("Data successfully sent to ThingSpeak");
  } else {
    Serial.println("Problem sending data to ThingSpeak. HTTP error code: " + String(responseCode));
  }

  // Blynk Virtual Pins Mapping
  Blynk.virtualWrite(V1, methanePPM);
  Blynk.virtualWrite(V2, co2PPM);
  Blynk.virtualWrite(V3, nh3PPM);
  Blynk.virtualWrite(V4, temperature);
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, spoiledRatio);

  // LCD Display Update
  lcd.clear();  // Clear LCD
  lcd.setCursor(0, 0);  // Set cursor to first row, first column
  lcd.print("Methane: ");
  lcd.print(methanePPM);

  lcd.setCursor(0, 1);  // Set cursor to second row, first column
  lcd.print("Spoil: ");
  lcd.print(foodStatus);  // Display Fresh or Spoiled

  if (foodStatus != previousFoodStatus) {
    String mqttMessage = "Food status: " + foodStatus;
    mqttClient.publish(mqttTopic, mqttMessage.c_str());
    Serial.println("Published to MQTT: " + mqttMessage);
    previousFoodStatus = foodStatus;
  }

  delay(20000);  // Delay between readings
}
