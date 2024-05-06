#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "weather";
const char* password = "12345678";

AsyncWebServer server(80);

// Sensor Pins
const int gasSensorPin = 34;   // MQ-2 Gas sensor
const int tempSensorPin = 32;  // DHT22 Temperature and Humidity sensor
const int lightSensorPin = 33; // Digital Light Sensor
const int rainSensorPin = 25;  // Rain Sensor
const int windSensorPin = 35;  // Anemometer

// DHT setup
DHT dht(tempSensorPin, DHT22);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(rainSensorPin, INPUT);
  // Configure the ESP32 as an AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  pinMode(lightSensorPin, INPUT); // Set the light sensor pin as input

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><title>IoT Based Weather Dashboard</title></head><body>"
                  "<h1>Environmental Sensor Readings</h1>"
                  "<p>Temperature: <span id='temperature'>Loading...</span>°C</p>"
                  "<p>Gas Concentration: <span id='gasValue'>Loading...</span> ppm</p>"
                  "<p>Light Level: <span id='lightLevel'>Loading...</span></p>"
                  "<p>Rain Status: <span id='rainStatus'>Loading...</span></p>"
                  "<p>Wind Speed: <span id='windSpeed'>Loading...</span> km/h</p>"
                  "<script>"
                  "setInterval(function() {"
                  "fetch('/sensors').then(response => response.json()).then(data => {"
                  "document.getElementById('temperature').innerText = data.temperature;"
                  "document.getElementById('gasValue').innerText = data.gas;"
                  "document.getElementById('lightLevel').innerText = data.light ? 'Dark / Night' : 'Bright / Day';"
                  "document.getElementById('rainStatus').innerText = data.rain ? 'Dry' : 'Rain';"
                  "document.getElementById('windSpeed').innerText = data.wind;"
                  "}).catch(error => console.error('Error:', error));"
                  "}, 1000);"
                  "</script>"
                  "</body></html>";
    request->send(200, "text/html", html);
  });

  // Endpoint to send sensor data
  server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest *request) {
    float temp = dht.readTemperature(); // Reading temperature
    int gasValue = analogRead(gasSensorPin); // Reading gas concentration
    bool lightLevel = digitalRead(lightSensorPin); // Reading light sensor
    bool isRaining = digitalRead(rainSensorPin); // Reading rain sensor
    float windSpeed = analogRead(windSensorPin) * 0.5; // Example conversion for wind speed

    String json = "{\"temperature\": \"" + String(temp, 1) + "\", \"gas\": \"" + String(gasValue) +
                  "\", \"light\": " + (lightLevel ? "true" : "false") +
                  ", \"rain\": " + (isRaining ? "true" : "false") +
                  ", \"wind\": \"" + String(windSpeed, 2) + "\"}";
    request->send(200, "application/json", json);
  });

  // Start server
  server.begin();
}

void loop() {
  int rainStatus = digitalRead(rainSensorPin);
  if (rainStatus == LOW) {
    Serial.println("It is raining");
  } else {
    Serial.println("It is not raining");
  }
  delay(1000); // Check every second
}
