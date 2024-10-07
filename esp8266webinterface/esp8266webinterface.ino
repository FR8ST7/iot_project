#include <ESP8266WiFi.h>

const char* ssid = "HOTSUPPORT";        
const char* password = "Emilia@143"; 

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Wi-Fi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  pinMode(LED_BUILTIN, OUTPUT);  
  digitalWrite(LED_BUILTIN, HIGH);  // Turn off LED (active low)
}

void loop() {
  WiFiClient client = server.available();   
  if (client) {                            
    Serial.println("New Client.");          
    String currentLine = "";                

    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    

        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("Click <a href=\"/H\">here</a> to turn the LED on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED off.<br>");
            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (active low)
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off (active low)
        }
      }
    }

    client.stop();
    Serial.println("Client Disconnected.");
  }
}
