#define BLYNK_TEMPLATE_ID "TMPL6iWRFjOf0"  // Replace with your actual Template ID
#define BLYNK_TEMPLATE_NAME "Soil Iot"     // Replace with your actual Template Name

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Blynk Auth Token and WiFi credentials
char auth[] = "mldp7zSmcQps3TyRsiYOk525SWJDZaIx"; // Replace with your Blynk Auth Token
char ssid[] = "Rasp";                             // Replace with your WiFi SSID
char pass[] = "123456799";                        // Replace with your WiFi password

int soilMoistureValue1 = 0;
int soilMoistureValue2 = 0;
int soilMoistureValue3 = 0;

BlynkTimer timer;

// Function to read soil moisture values from Blynk virtual pins V1, V2, and V3
BLYNK_WRITE(V1) {
  soilMoistureValue1 = param.asDouble(); // Read value from V1
  Serial.print("Soil Moisture 1: ");
  Serial.print(soilMoistureValue1);
  Serial.println("%");
}

BLYNK_WRITE(V2) {
  soilMoistureValue2 = param.asDouble(); // Read value from V2
  Serial.print("Soil Moisture 2: ");
  Serial.print(soilMoistureValue2);
  Serial.println("%");
}

BLYNK_WRITE(V3) {
  soilMoistureValue3 = param.asDouble(); // Read value from V3
  Serial.print("Soil Moisture 3: ");
  Serial.print(soilMoistureValue3);
  Serial.println("%");
}

void setup() {
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run(); // Run Blynk
}
