int pin=7;

void setup(){
  pinMode(pin,INPUT);
  pinMode(8,OUTPUT);
  Serial.begin(9600);
}

void loop(){
  int state= digitalRead(pin);
  if(state==HIGH){
    digitalWrite(8,HIGH);
    Serial.println("motion detected.");
  }
  else{
    digitalWrite(8,LOW);
    Serial.println("no motion detected.");
  }
  delay(500);
}

