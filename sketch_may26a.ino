const int SOUND_SENSOR_PIN = A0;
int sensorValue;
 
void setup() {
  Serial.begin(9600);
}
 
void loop() {
  sensorValue = analogRead(SOUND_SENSOR_PIN);
  Serial.println(sensorValue);
  delay(70);
}
