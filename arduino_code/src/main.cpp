/*
Capacitive Soil Moisture Sensor WiFI Read In
Jackson Mayfield - 09/06/2024
*/

#include <Arduino.h>
#include <SoftwareSerial.h>

// Define SoftwareSerial pins for communication with ESP8266
SoftwareSerial esp8266(2, 3);  // RX, TX

// Define the sensor pin (e.g., analog pin A0)
const int sensorPin = A0;

//Sensor Constants
const int dry = 463; // Constant for dry sensor
const int wet = 195; // Constant for wet sensor

//Server ID vars
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* host = "your_server_ip_or_hostname";  // IP or domain of Django server
const int httpPort = 8000;  // Port for Django server

// Helper function to send AT commands to ESP8266
void sendATCommand(const char* cmd, const char* success_response, int wait_time = 2000) {
    esp8266.println(cmd);
    long int time = millis();
    while ((millis() - time) < wait_time) {
        while (esp8266.available()) {
            String response = esp8266.readString();
            Serial.println(response);
            if (response.indexOf(success_response) != -1) {
                return;
            }
        }
    }
}

void setup() {
    // Start the serial communication for debugging and ESP8266 communication
    Serial.begin(115200);   // For Arduino serial monitor
    esp8266.begin(115200);  // For ESP8266 (adjust baud rate if necessary)

    // Initialize ESP8266 and connect to Wi-Fi
    Serial.println("Initializing ESP8266...");

    sendATCommand("AT", "OK");  // Check communication with ESP8266
    sendATCommand("AT+CWMODE=1", "OK");  // Set Wi-Fi mode to client
    String connectWiFi = String("AT+CWJAP=\"") + ssid + "\",\"" + password + "\"";
    sendATCommand(connectWiFi.c_str(), "WIFI CONNECTED", 10000);  // Connect to Wi-Fi
}

// the loop routine runs over and over again forever:
void loop() {
  // Read Sensor Data
  int sensor_val = analogRead(sensorPin);
  int per_moisture = map(sensor_val, wet, dry, 100, 0); // Translates the analog read to a percentage between 0 - 100% moisture content
  String moisture_str = String(per_moisture); // Convert the integer to a String and concatenate with other text
  Serial.println("Moisture level: " + moisture_str + "%");
  
  // Connect to the server
  String connectCmd = String("AT+CIPSTART=\"TCP\",\"") + host + "\"," + httpPort;
  sendATCommand(connectCmd.c_str(), "CONNECT");

  // Prepare the HTTP POST request
  String httpRequest = "POST /recieve-data/ HTTP/1.1\r\n";
  httpRequest += "Host: " + String(host) + "\r\n";
  httpRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
  String postData = "sensorData=" + String(sensor_val);
  httpRequest += "Content-Length: " + String(postData.length()) + "\r\n\r\n";
  httpRequest += postData;

  // Send the HTTP request length
  String sendCmd = "AT+CIPSEND=" + String(httpRequest.length());
  sendATCommand(sendCmd.c_str(), ">");

  // Send the actual HTTP request
  esp8266.print(httpRequest);
  delay(1000);  // Give the ESP8266 some time to process

  // Read the response from the server
  while (esp8266.available()) {
      String response = esp8266.readString();
      Serial.println("Server Response: ");
      Serial.println(response);
  }

  // Close the connection
  sendATCommand("AT+CIPCLOSE", "CLOSED");

  // Wait 10 seconds before sending data again
  delay(10000);
}
