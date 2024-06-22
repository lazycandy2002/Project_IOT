#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_TEMPLATE_ID "TMPL6iWRFjOf0"
#define BLYNK_TEMPLATE_NAME "Soil Iot"

const int soilMoisturePin = A0;
const int relayPin1 = D3;  // Water Relay
const int relayPin2 = D4;  // Power Relay

int soilMoistureValue = 0;
int soilMoistureThreshold = 25;  // Adjust as needed
int soilMoistureThresholdMAX = 30;  // Adjust as needed
bool manualControl1 = false;
bool manualControl2 = false;
bool isSoilDry = false;
bool isSoilWet = false;

// Blynk Auth Token
char auth[] = "mldp7zSmcQps3TyRsiYOk525SWJDZaIx";  // Replace with your Blynk Auth Token

// Your WiFi credentials
char ssid[] = "Rasp";  // Replace with your WiFi SSID
char pass[] = "123456799";  // Replace with your WiFi password

BlynkTimer timer;

// Blynk virtual pin handlers for relay 1 (Water pump)
BLYNK_WRITE(V1) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    manualControl1 = true;
    digitalWrite(relayPin1, HIGH);
    Serial.println("Manual control: Relay 1 (Water pump) is ON.");
  } else {
    manualControl1 = false;
    digitalWrite(relayPin1, LOW);
    Serial.println("Manual control: Relay 1 (Water pump) is OFF.");
  }
}

// Blynk virtual pin handlers for relay 2 (Power)
BLYNK_WRITE(V0) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    manualControl2 = true;
    Blynk.virtualWrite(V3, 1);
    digitalWrite(relayPin2, HIGH);
    Serial.println("Manual control: Relay 2 (Power) is ON.");
  } else {
    manualControl2 = false;
    Blynk.virtualWrite(V3, 0);
    digitalWrite(relayPin2, LOW);
    Serial.println("Manual control: Relay 2 (Power) is OFF.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(soilMoisturePin, INPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every second
  timer.setInterval(1000L, readSoilMoisture);
}

void loop() {
  Blynk.run();
  timer.run();
}

void readSoilMoisture() {
  if (!manualControl1) {
    soilMoistureValue = analogRead(soilMoisturePin);
    int soilMoisturePercent = map(soilMoistureValue, 0, 1023, 100, 0);
    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");

    // Send soil moisture value to Blynk app
    Blynk.virtualWrite(V5, soilMoisturePercent);

    if (soilMoisturePercent < soilMoistureThreshold && !isSoilDry) {
      digitalWrite(relayPin1, HIGH);
      Serial.println("Soil is dry. Water pump is ON.");
      Blynk.virtualWrite(V1, 1);
      Blynk.logEvent("dry_soil_notification","Soil Moisture Low! Pump starting...");
      isSoilDry = true;
      isSoilWet = false;
    } else if (soilMoisturePercent > soilMoistureThresholdMAX && !isSoilWet){
      digitalWrite(relayPin1, LOW);
      Serial.println("Soil is wet. Water pump is OFF.");
      Blynk.virtualWrite(V1, 0);
      Blynk.logEvent("wet_soil_notification","Plant soil moisturized!, Pump turned off...");
      isSoilWet = true;
      isSoilDry = false;
    }
  }
}
