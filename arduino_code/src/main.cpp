#include <Arduino.h>

/*
Capacitive Soil Moisture Sensor Read In
*/
const int dry = 463; // Constant for dry sensor
const int wet = 195; // Constant for wet sensor

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  int sensor_val = analogRead(A0);
  // Translates the analog read to a percentage between 0 - 100% moisture content
  int per_moisture = map(sensor_val, wet, dry, 100, 0);
  // Convert the integer to a String and concatenate with other text
  String moisture_str = String(per_moisture);
  Serial.println("Moisture level: " + moisture_str + "%");
  delay(1000);
}
