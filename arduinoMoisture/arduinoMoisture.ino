void setup() {
Serial.begin(9600);

}

void loop() {
long sensor= analogRead(A0);
float percentage= (100-((sensor/1023.00)*100));

Serial.println("Moisture(%):");
Serial.println(percentage);

delay(1000);
}
