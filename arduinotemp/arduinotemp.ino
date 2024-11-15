float sensorinput;
float temp;
float volt;

void setup(){
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(8,OUTPUT);
  Serial.begin(9600);
}
void loop(){
  sensorinput= analogRead(A0);
  volt= sensorinput*4.88;
  temp= volt/10;
  Serial.print("temperature(c):");
  Serial.println(temp);
  
  if(temp<=10.00){
    digitalWrite(10,HIGH);
    digitalWrite(9,LOW);
    digitalWrite(8,LOW);
  }
  else if(temp >=10.00 && temp <=15.00) {
    digitalWrite(10,LOW);
    digitalWrite(9,HIGH);
    digitalWrite(8,LOW);
  }
  else if (temp>20.00){
    digitalWrite(10,LOW);
    digitalWrite(9,LOW);
    digitalWrite(8,HIGH);
  }
  delay(500);
  }


