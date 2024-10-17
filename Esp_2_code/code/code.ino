#define BLYNK_TEMPLATE_ID "TMPL6iWRFjOf0"  // Replace with your actual Template ID
#define BLYNK_TEMPLATE_NAME "Soil Iot"     // Replace with your actual Template Name

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


const int soilMoisturePin = A0;  // Soil Moisture Pin

int soilMoistureValue = 0;
int soilMoisturePercent = 0;

// Blynk Auth Token
char auth[] = "mldp7zSmcQps3TyRsiYOk525SWJDZaIx";  // Replace with your Blynk Auth Token

// Your WiFi credentials
char ssid[] = "Rasp";  // Replace with your WiFi SSID
char pass[] = "123456799";  // Replace with your WiFi password

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  pinMode(soilMoisturePin, INPUT);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSoilMoisture);
}

void loop() {
  Blynk.run();
  timer.run();
}

// Function to read soil moisture and send to Blynk V2
void sendSoilMoisture() {
  soilMoistureValue = analogRead(soilMoisturePin);
  soilMoisturePercent = map(soilMoistureValue, 0, 1023, 100, 0);
  
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.println("%");

  // Send soil moisture value to Blynk virtual pin V2
  Blynk.virtualWrite(V2, soilMoisturePercent);
}
