// this is for the internal temperature sensor, if you want to check the device temperature use it.
// otherwise please use a DHT11 or 22 for more accurate temperature readings.

void setup() {
  Serial.begin(115200);
}

void loop() {
  float temp_celsius = temperatureRead();

  Serial.print("Temp onBoard ");
  Serial.print(temp_celsius);
  Serial.println("°C");

  delay(1000);
}