void setup() {
  pinMode(7,OUTPUT);
  pinMode(6,INPUT);
}

void loop() {
  int button=digitalRead(2);
  digitalWrite(7,LOW);
  if(button==HIGH)
  {
    digitalWrite(7,LOW);
  }
  else{
  digitalWrite(7,HIGH);
  }
}
