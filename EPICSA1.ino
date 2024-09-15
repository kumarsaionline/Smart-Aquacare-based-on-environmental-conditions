#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

// DS18B20 sensor
#define ONE_WIRE_BUS 25
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// pH sensor
const int pH_Pin = 33;

// Turbidity sensor
const int TurbidityPin = 32;



// WiFi credentials
const char* ssid = "ai";
const char* password = "";

// ThingSpeak configuration
const char* serverName = "http://api.thingspeak.com/update";
String apiKey = "L9IRYKGW5JZV3Y64"; // Replace with your actual ThingSpeak API key

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  sensors.begin();
  pinMode(pH_Pin, INPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    delay(10000);

    // DS18B20 sensor
    sensors.requestTemperatures();
    float temperatureCelsius = sensors.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.print(temperatureCelsius);
    Serial.println(" °C");

    // pH sensor
    float pHValue = readpH();
    Serial.print("pH: ");
    Serial.println(pHValue);

    // Turbidity sensor
    float turbidity = readTurbidity();
    Serial.print("Turbidity: ");
    Serial.println(turbidity);

    // Send data to ThingSpeak
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(temperatureCelsius) + "&field2=" + String(pHValue) + "&field3=" + String(turbidity);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println("Data successfully sent to ThingSpeak");
    http.end();
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
    connectToWiFi();
  }

  delay(5000); // Adjust the delay based on your requirements
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println("");
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi. Please check your credentials.");
  }
}

float readpH() {
  int pHValue = analogRead(pH_Pin);
  float voltage = pHValue * (3.3 / 4095.0);
  // You may need to calibrate this conversion based on your pH sensor's characteristics.
  return 3.3*voltage;
}

float readTurbidity() {
  int rawValue = analogRead(TurbidityPin);
  float voltage = rawValue * (3.3 / 4095.0);
  // You may need to calibrate this conversion based on your turbidity sensor's characteristics.
  return 3.3*voltage;
}
