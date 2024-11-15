long duration;
float distance;

void setup(){
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(7,OUTPUT);//TRIG
  pinMode(6,INPUT);//ECHO
  Serial.begin(9600);
}
void loop(){
  digitalWrite(7,LOW);
  delayMicroseconds(2);
  digitalWrite(7,HIGH);
  delayMicroseconds(10);
  digitalWrite(7,LOW);

  duration= pulseIn(6,HIGH);
  distance= duration*(0.034/2);

  Serial.print("distance:");
  Serial.println(distance);
  
  if(distance<=30.00){
    digitalWrite(10,HIGH);
    digitalWrite(9,LOW);
    digitalWrite(8,LOW);
  }
  else if(distance >=30.00 && distance <=100.00) {
    digitalWrite(10,LOW);
    digitalWrite(9,HIGH);
    digitalWrite(8,LOW);
  }
  else if (distance>100.00){
    digitalWrite(10,LOW);
    digitalWrite(9,LOW);
    digitalWrite(8,HIGH);
  }
  delay(500);
  }


