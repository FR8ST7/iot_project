// Pin definitions for MQ2
int mq2AnalogPin = A0;  // MQ2 analog output
int mq2DigitalPin = 2;  // MQ2 digital output

// Pin definitions for MQ135
int mq135AnalogPin = A1;  // MQ135 analog output
int mq135DigitalPin = 3;  // MQ135 digital output

// LED pin definitions
int led1 = 4;  // LED 1 connected to pin 4
int led2 = 8;  // LED 2 connected to pin 5
int led3 = 7;  // LED 3 connected to pin 6

void setup() {
  Serial.begin(9600);        // Start serial communication
  pinMode(mq2DigitalPin, INPUT);  // Set MQ2 digital pin as input
  pinMode(mq135DigitalPin, INPUT); // Set MQ135 digital pin as input

  // Set LED pins as output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}

void loop() {
  // MQ2 Sensor: Reading analog value and converting to gas concentration
  int mq2AnalogValue = analogRead(mq2AnalogPin);
  float mq2GasPPM = 1000 * (mq2AnalogValue / 1023.0);  // Estimated total gas concentration for MQ2
  
  // MQ135 Sensor: Reading analog value and converting to gas concentration
  int mq135AnalogValue = analogRead(mq135AnalogPin);
  float mq135GasPPM = 1000 * (mq135AnalogValue / 1023.0);  // Estimated total gas concentration for MQ135

  // Break down MQ2 readings into individual gases (rough percentages)
  float methanePPM = mq2GasPPM * 0.40;
  float lpgPPM = mq2GasPPM * 0.30;
  float hydrogenPPM = mq2GasPPM * 0.10;
  float coPPM = mq2GasPPM * 0.10;
  float smokePPM = mq2GasPPM * 0.10;

  // Break down MQ135 readings into individual gases (rough percentages)
  float co2PPM = mq135GasPPM * 0.40;
  float nh3PPM = mq135GasPPM * 0.20;
  float noxPPM = mq135GasPPM * 0.15;
  float alcoholPPM = mq135GasPPM * 0.15;
  float benzenePPM = mq135GasPPM * 0.10;

  // Display individual gas concentrations for MQ2
  Serial.print("MQ2 - Methane (CH4): ");
  Serial.print(methanePPM);
  Serial.print(" ppm, LPG: ");
  Serial.print(lpgPPM);
  Serial.print(" ppm, Hydrogen (H2): ");
  Serial.print(hydrogenPPM);
  Serial.print(" ppm, CO: ");
  Serial.print(coPPM);
  Serial.print(" ppm, Smoke: ");
  Serial.println(smokePPM);
  
  // Display individual gas concentrations for MQ135
  Serial.print("MQ135 - CO2: ");
  Serial.print(co2PPM);
  Serial.print(" ppm, NH3: ");
  Serial.print(nh3PPM);
  Serial.print(" ppm, NOx: ");
  Serial.print(noxPPM);
  Serial.print(" ppm, Alcohol: ");
  Serial.print(alcoholPPM);
  Serial.print(" ppm, Benzene: ");
  Serial.println(benzenePPM);

  // LED pattern logic
  // If Methane or CO2 is high, blink LED1 rapidly
  if (methanePPM > 100 || co2PPM > 200) {
    digitalWrite(led1, HIGH);
    delay(100);
    digitalWrite(led1, LOW);
    delay(100);
  }
  else {
    digitalWrite(led1, LOW);
  }

  // If LPG or NH3 is moderate, blink LED2 slowly
  if (lpgPPM > 100 || nh3PPM > 10) {
    digitalWrite(led2, HIGH);
    delay(500);
    digitalWrite(led2, LOW);
    delay(500);
  }
  else {
    digitalWrite(led2, LOW);
  }

  // If Hydrogen or NOx is detected, blink LED3 in a different pattern
  if (hydrogenPPM > 30 || noxPPM > 100) {
    digitalWrite(led3, HIGH);
    delay(300);
    digitalWrite(led3, LOW);
    delay(300);
  }
  else {
    digitalWrite(led3, LOW);
  }

  delay(1000);  // Delay for 1 second before the next reading
}
