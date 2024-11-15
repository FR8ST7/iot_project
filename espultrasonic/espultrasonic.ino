long duration;
float distance;

void setup() {
  pinMode(D5, OUTPUT); 
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT); 
  pinMode(D1, OUTPUT); // TRIG pin, GPIO5
  pinMode(D2, INPUT);  // ECHO pin, GPIO4
  Serial.begin(115200); 
}

void loop() {
  digitalWrite(D1, LOW);
  delayMicroseconds(2);
  digitalWrite(D1, HIGH);
  delayMicroseconds(10);
  digitalWrite(D1, LOW);

  duration = pulseIn(D2, HIGH);
  distance = duration * (0.034 / 2);

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance <= 30.00) {
    digitalWrite(D5, HIGH);   
    digitalWrite(D6, LOW);    
    digitalWrite(D7, LOW);   
  }
  else if (distance > 30.00 && distance <= 100.00) {
    digitalWrite(D5, LOW);    
    digitalWrite(D6, HIGH);   
    digitalWrite(D7, LOW);    
  }
  else if (distance > 100.00) {
    digitalWrite(D5, LOW);  
    digitalWrite(D6, LOW);    
    digitalWrite(D7, HIGH);   
  }

  delay(500);
}
