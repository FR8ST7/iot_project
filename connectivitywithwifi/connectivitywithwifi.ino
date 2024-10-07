#include <ESP8266WiFi.h>

const char* ssid = "HOTSUPPORT";        // Replace with your Wi-Fi network name
const char* password = "Emilia@143"; // Replace with your Wi-Fi password

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Wi-Fi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Nothing to do here
}
