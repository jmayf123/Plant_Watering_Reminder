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

}

void loop() {
    // Read Sensor Data
    int sensor_val = analogRead(sensorPin);
    int per_moisture = map(sensor_val, wet, dry, 100, 0);
    String moisture_str = String(per_moisture);
    Serial.println("Moisture level: " + moisture_str + "%");

    // Prepare the POST data
    String postData = "sensor_value=" + moisture_str;

    // Check WiFi connection status
    int wifiStatus = WiFi.status();

    if (wifiStatus != WL_CONNECTED) {
        Serial.print("WiFi Status: ");
        Serial.println(wifiStatus);  // Print the current WiFi status
        Serial.println("Trying to connect to WiFi...");
        
        // Attempt to reconnect
        WiFi.begin(ssid, password);
        
        // Wait for a while to check if connected
        unsigned long startAttemptTime = millis();
        while (wifiStatus != WL_CONNECTED && millis() - startAttemptTime < 10000) { // Timeout after 10 seconds
            delay(500);
            Serial.print(".");
            wifiStatus = WiFi.status();  // Update status
        }

        if (wifiStatus == WL_CONNECTED) {
            Serial.println("Connected to WiFi");
        } else {
            Serial.println("Failed to connect to WiFi. Retrying...");
        }
    }

    // Send data to the Django server only if connected to WiFi
    if (client.connect(server, port)) {
        Serial.println("Connected to server");
        
        // Start POST request
        client.println("POST /sensor/ HTTP/1.1");
        client.println("Host: " + String(server));
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println("Connection: close");
        client.print("Content-Length: ");
        client.println(postData.length());
        client.println();
        client.println(postData);
        client.stop(); // Close the connection
    } else {
        Serial.println("Failed to connect to server, retrying...");
    }

    // Wait for 10 seconds before sending the next data
    delay(2000);
}



