/*
Capacitive Soil Moisture Sensor WiFI Read In
Jackson Mayfield - 09/06/2024
*/
//////////////////////////////////////////////////////////////////////////////////////
////////// Setup 
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <WiFiEspAT.h>
#include <WiFiClient.h>
#include "secrets.h"


// Define SoftwareSerial pins for communication with ESP8266
SoftwareSerial esp8266(2, 3);  // RX, TX

// Define the sensor pin (e.g., analog pin A0)
const int sensorPin = A0;

//Sensor Constants
const int dry = 463; // Constant for dry sensor
const int wet = 195; // Constant for wet sensor

// Network ID vars
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* server = "192.168.1.149";  // Replace with your Django server address
const int port = 8000;  // Assuming you're not using HTTPS

WiFiClient client;

//////////////////////////////////////////////////////////////////////////////////////
////////// Main Code

void setup() {
  // Start the serial communication for debugging and ESP8266 communication
  Serial.begin(115200);   // For Arduino serial monitor
  esp8266.begin(115200);  // For ESP8266 (adjust baud rate if necessary)
  
  // Set up soil moisture sensor pin
  pinMode(sensorPin, INPUT);
  
  // Initialize ESP AT commands library
  WiFi.init(esp8266);

  // Check for the presence of the module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ESP8266 module not detected");
    while (true);  // Stop if no module
  }

  // Attempt to connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");

  // Serial.println("Testing connection to server...");
  // if (client.connect(server, port)) {
  //   Serial.println("Connected to server");
  //   client.print("Host: ");
  //   client.println(server);
  //   client.println("Connection: close");
  //   client.println();
  //   client.flush();
  // }
}

void loop() {
  // Read Sensor Data
  int sensor_val = analogRead(sensorPin);
  int per_moisture = map(sensor_val, wet, dry, 100, 0); // Translates the analog read to a percentage between 0 - 100% moisture content
  String moisture_str = String(per_moisture); // Convert the integer to a String and concatenate with other text
  Serial.println("Moisture level: " + moisture_str + "%");
  
  // Prepare the POST data
  String postData = "sensor_value=" + moisture_str;

  // Send data to the Django server
  if (client.connect(server, port)) {
    Serial.println("Connected to server");

    // Start POST request
    client.println("POST /sensor/ HTTP/1.1");  // Replace /sensor with the correct endpoint URL on your Django server
    client.println("Host: " + String(server));
    client.println("Content-Type: application/x-www-form-urlencoded");  // Form data encoding
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(postData.length());  // Send the length of the POST data
    client.println();  // End of headers
    client.println(postData);  // Send the POST data (sensor value) 
    client.stop();  // Close the connection
  } else {
    Serial.println("Failed to connect to server");
  }

  // Wait for 10 seconds before sending the next data
  delay(10000);
}

