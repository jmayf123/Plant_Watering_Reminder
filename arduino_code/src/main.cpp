/*
Capacitive Soil Moisture Sensor WiFI Read In
Jackson Mayfield - 09/06/2024
*/
//////////////////////////////////////////////////////////////////////////////////////
////////// Setup 
#include <Arduino.h>
#include <SoftwareSerial.h>

// Function prototypes
void connectToWiFi();
void sendDataOverWiFi(int data);
bool sendATCommand(const char* cmd, const char* expectedResponse, unsigned long timeout);

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
const char* host = "192.168.1.149";  // IP or domain of Django server
const int port = 80;  // Port for Django server


//////////////////////////////////////////////////////////////////////////////////////
////////// Main Code

void setup() {
    // Start the serial communication for debugging and ESP8266 communication
    Serial.begin(9600);   // For Arduino serial monitor
    esp8266.begin(115200);  // For ESP8266 (adjust baud rate if necessary)

  // Initialize ESP8266
  sendATCommand("AT", "OK", 1000);            // Check if ESP is ready
  sendATCommand("AT+CWMODE=1", "OK", 1000);   // Set mode to station
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

//////////////////////////////////////////////////////////////////////////////////////
////////// Helper Functions

void connectToWiFi() {
  String cmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"";
  sendATCommand(cmd.c_str(), "OK", 5000);    // Connect to WiFi network
}

void sendDataOverWiFi(int sensorValue) {
  String httpRequest = "GET /update?data=" + String(sensorValue) + " HTTP/1.1\r\n";
  httpRequest += "Host: " + String(host) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";

  // Start the connection
  String cmd = "AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(port);
  if (sendATCommand(cmd.c_str(), "OK", 2000)) {
    delay(1000);

    // Send data length
    cmd = "AT+CIPSEND=" + String(httpRequest.length());
    if (sendATCommand(cmd.c_str(), ">", 2000)) {
      esp8266.print(httpRequest);  // Send the actual HTTP request
      delay(1000);
      esp8266.println("AT+CIPCLOSE");  // Close the connection
    }
  }
}

// Function to send AT commands and wait for a response
bool sendATCommand(const char* cmd, const char* expectedResponse, unsigned long timeout) {
  esp8266.println(cmd);
  unsigned long startTime = millis();
  String response = "";
  
  while (millis() - startTime < timeout) {
    while (esp8266.available()) {
      char c = esp8266.read();    // Read character by character
      response += c;

      // Check if the expected response is found in the current response string
      if (response.indexOf(expectedResponse) != -1) {
        return true;  // Found the expected response
      }
    }
  }
  Serial.println("Failed: " + String(cmd));  // Failed to find the expected response
  return false;
}