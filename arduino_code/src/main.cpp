/*
Capacitive Soil Moisture Sensor WiFI Read In
Jackson Mayfield - 09/06/2024
*/
//////////////////////////////////////////////////////////////////////////////////////
////////// Setup 
#include <Arduino.h>
#include <SoftwareSerial.h>

// Define SoftwareSerial pins for communication with ESP8266
SoftwareSerial esp8266(2, 3);  // RX, TX

// Define the sensor pin (e.g., analog pin A0)
const int sensorPin = A0;

//Sensor Constants
const int dry = 463; // Constant for dry sensor
const int wet = 195; // Constant for wet sensor

// Network ID vars
const char* ssid = "saltlakecity";
const char* password = "Paellalover22";
const char* host = "0.0.0.0";  // IP or domain of Django server
String port = "8000";  // Port for Django server

//////////////////////////////////////////////////////////////////////////////////////
////////// Helper Functions
void connectToWiFi() {
  // Send AT command to reset ESP8266
  esp8266.println("AT+RST");
  delay(2000);
  esp8266.println("AT+CWMODE_CUR=1");  // Set ESP8266 to client mode
  delay(2000);

  // Connect to WiFi
  String cmd = "AT+CWJAP_CUR=\"" + String(ssid) + "\",\"" + String(password) + "\"";
  esp8266.println(cmd);
  delay(5000);

  // Check if connection is successful
  if (esp8266.find("OK")) {
    Serial.println("Connected to WiFi");
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void sendDataOverWiFi(int sensorValue) {
  String cmd = "AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(port);
  esp8266.println(cmd);
  delay(2000);
  
  if (esp8266.find("OK")) {
    String httpPacket = "GET /update?moisture=" + String(sensorValue) + " HTTP/1.1\r\nHost: " + String(host) + "\r\nConnection: close\r\n\r\n";

    // Send length of the HTTP packet
    cmd = "AT+CIPSEND=" + String(httpPacket.length());
    esp8266.println(cmd);
    delay(2000);

    // Send HTTP packet
    esp8266.print(httpPacket);
    delay(2000);

    // Close the connection
    esp8266.println("AT+CIPCLOSE");
    delay(1000);
  } else {
    Serial.println("Failed to connect to server");
  }
}

//////////////////////////////////////////////////////////////////////////////////////
////////// Main Code

void setup() {
  // Start the serial communication for debugging and ESP8266 communication
  Serial.begin(115200);   // For Arduino serial monitor
  esp8266.begin(115200);  // For ESP8266 (adjust baud rate if necessary)
  
  // Set up soil moisture sensor pin
  pinMode(sensorPin, INPUT);
  
  // Initialize ESP8266
  connectToWiFi();                            // Connect to WiFi
}

void loop() {
  // Read Sensor Data
  int sensor_val = analogRead(sensorPin);
  int per_moisture = map(sensor_val, wet, dry, 100, 0); // Translates the analog read to a percentage between 0 - 100% moisture content
  String moisture_str = String(per_moisture); // Convert the integer to a String and concatenate with other text
  Serial.println("Moisture level: " + moisture_str + "%");
  
  // Send Data to the Server
  sendDataOverWiFi(sensor_val);
  delay(3000);  // Delay before next reading
}

