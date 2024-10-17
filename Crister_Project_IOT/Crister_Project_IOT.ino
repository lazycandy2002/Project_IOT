#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp32.h>

// Blynk settings
#define BLYNK_TEMPLATE_ID "TMPL6iWRFjOf0" // Template ID
#define BLYNK_TEMPLATE_NAME "Soil IoT"      // Template Name
char auth[] = "mldp7zSmcQps3TyRsiYOk525SWJDZaIx"; // Blynk Auth Token
char ssid[] = "PLDTHOMEFIBRdNvwT";                             // WiFi SSID
char pass[] = "PLDTWIFIErvgj";                           // WiFi Password

// Relay Pins
#define relayPin2 12 // Controls Soil Moisture 1
#define relayPin3 13 // Controls Soil Moisture 2
#define relayPin4 14 // Controls Soil Moisture 3

// Soil Moisture Sensor Pins
#define soilMoisturePin1 4    // GPIO 4 (ADC1_2)
#define soilMoisturePin2 5    // GPIO 5 (ADC2_4)
#define soilMoisturePin3 6    // GPIO 6 (ADC1_5)

// NeoPixel on GPIO 48 for LED control
#define LED_PIN 48    // NeoPixel data pin
#define NUMPIXELS 1   // Number of NeoPixels used

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// LCD using hd44780 library
hd44780_I2Cexp lcd; // Declare lcd object: auto locates and initializes the LCD

// Variables
int soilMoistureValue1, soilMoistureValue2, soilMoistureValue3;
int soilMoistureThreshold = 20;    // Threshold to turn on relay
int soilMoistureThresholdMAX = 30; // Threshold to turn off relay
bool isConnected = false; // To track WiFi connection state

// Soil moisture states
bool isSoilDry1 = false;
bool isSoilWet1 = false;
bool isSoilDry2 = false;
bool isSoilWet2 = false;
bool isSoilDry3 = false;
bool isSoilWet3 = false;

// Timer variable
unsigned long runtime = 0; // Runtime in seconds

void setup() {
    Serial.begin(115200);

    // Initialize NeoPixel
    pixels.begin();
    pixels.setBrightness(50); // Adjust brightness level

    // Initialize Blynk
    Blynk.begin(auth, ssid, pass);

    // Initialize relays
    pinMode(relayPin2, OUTPUT);  // Soil Moisture 1 relay
    pinMode(relayPin3, OUTPUT);  // Soil Moisture 2 relay
    pinMode(relayPin4, OUTPUT);  // Soil Moisture 3 relay

    // Initialize sensors
    pinMode(soilMoisturePin1, INPUT);
    pinMode(soilMoisturePin2, INPUT);
    pinMode(soilMoisturePin3, INPUT);

    // Initialize LCD
    lcd.begin(16, 4);  // Initialize the display for 16x4 size
    lcd.clear();
    lcd.print("Soil Moisture System");

    // Set initial states
    digitalWrite(relayPin2, LOW); // Ensure relayPin2 is OFF
    digitalWrite(relayPin3, LOW); // Ensure relayPin3 is OFF
    digitalWrite(relayPin4, LOW); // Ensure relayPin4 is OFF

    // Initialize Blynk with the current states
    Blynk.virtualWrite(V4, 0); // Send initial state for relayPin2
    Blynk.virtualWrite(V5, 0); // Send initial state for relayPin3
    Blynk.virtualWrite(V6, 0); // Send initial state for relayPin4

    delay(2000); // Show initial message for 2 seconds

    connectToWiFi(); // Connect to WiFi
}

void loop() {
    Blynk.run();
    updateLED();

    // Increment the runtime every second
    runtime++;
    Blynk.virtualWrite(V0, runtime / 60); // Upload runtime to V0
    delay(1000); // Wait for 1 second

    // Read and control moisture sensors continuously
    readSoilMoisture();
}

void connectToWiFi() {
    Serial.printf("Connecting to %s ", ssid);
    lcd.clear();
    lcd.print("Connecting to ");
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    delay(1000);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");
    lcd.clear();
    lcd.print("CONNECTED");
    delay(2000);
}

void updateLED() {
    if (WiFi.status() != WL_CONNECTED) {
        if (isConnected) {
            isConnected = false;
            // Turn LED Red when not connected to WiFi
            pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Red
            pixels.show();
        }
    } else {
        if (!isConnected) {
            isConnected = true;
            // If connected, show Green
            pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Green
            pixels.show();
        }
    }
}

void readSoilMoisture() {
    // Read values
    soilMoistureValue1 = analogRead(soilMoisturePin1);
    soilMoistureValue2 = analogRead(soilMoisturePin2);
    soilMoistureValue3 = analogRead(soilMoisturePin3);

    int soilMoisturePercent1 = map(soilMoistureValue1, 0, 4095, 100, 0);
    int soilMoisturePercent2 = map(soilMoistureValue2, 0, 4095, 100, 0);
    int soilMoisturePercent3 = map(soilMoistureValue3, 0, 4095, 100, 0);

    // Send values to Blynk
    Blynk.virtualWrite(V1, soilMoisturePercent1);
    Blynk.virtualWrite(V2, soilMoisturePercent2);
    Blynk.virtualWrite(V3, soilMoisturePercent3);

    // Update LCD Display
    lcd.clear();
    // Display runtime on the LCD in HH:MM:SS format
    int hours = runtime / 3600;
    int minutes = (runtime % 3600) / 60;
    int seconds = runtime % 60;
    lcd.setCursor(0, 0);
    lcd.print("Run Time: ");
    lcd.print(hours);
    lcd.print(":");
    lcd.print(minutes < 10 ? "0" : ""); // Leading zero for minutes
    lcd.print(minutes);
    lcd.print(":");
    lcd.print(seconds < 10 ? "0" : ""); // Leading zero for seconds
    lcd.print(seconds);

    // Display soil moisture percentages
    lcd.setCursor(0, 1);
    lcd.print("Soil 1: ");
    lcd.print(soilMoisturePercent1);
    lcd.print("%");

    lcd.setCursor(0, 2);
    lcd.print("Soil 2: ");
    lcd.print(soilMoisturePercent2);
    lcd.print("%");

    lcd.setCursor(0, 3);
    lcd.print("Soil 3: ");
    lcd.print(soilMoisturePercent3);
    lcd.print("%");

    // Control relays based on soil moisture 1
    if (soilMoisturePercent1 < soilMoistureThreshold && !isSoilDry1) {
        digitalWrite(relayPin2, HIGH);
        Blynk.virtualWrite(V4, 1);
        isSoilDry1 = true;
        isSoilWet1 = false;

        // Log event for low soil moisture
        Blynk.logEvent("dry_soil_notification", "Soil Moisture 1 Low! Pump starting...");
    } else if (soilMoisturePercent1 > soilMoistureThresholdMAX && !isSoilWet1) {
        digitalWrite(relayPin2, LOW);
        Blynk.virtualWrite(V4, 0);
        isSoilWet1 = true;
        isSoilDry1 = false;

        // Log event for soil moisturized
        Blynk.logEvent("wet_soil_notification", "Plant soil 1 moisturized! Pump turned off...");
    }

    // Control relays based on soil moisture 2
    if (soilMoisturePercent2 < soilMoistureThreshold && !isSoilDry2) {
        digitalWrite(relayPin3, HIGH);
        Blynk.virtualWrite(V5, 1);
        isSoilDry2 = true;
        isSoilWet2 = false;

        // Log event for low soil moisture
        Blynk.logEvent("dry_soil_notification", "Soil Moisture 2 Low! Pump starting...");
    } else if (soilMoisturePercent2 > soilMoistureThresholdMAX && !isSoilWet2) {
        digitalWrite(relayPin3, LOW);
        Blynk.virtualWrite(V5, 0);
        isSoilWet2 = true;
        isSoilDry2 = false;

        // Log event for soil moisturized
        Blynk.logEvent("wet_soil_notification", "Plant soil 2 moisturized! Pump turned off...");
    }

    // Control relays based on soil moisture 3
    if (soilMoisturePercent3 < soilMoistureThreshold && !isSoilDry3) {
        digitalWrite(relayPin4, HIGH);
        Blynk.virtualWrite(V6, 1);
        isSoilDry3 = true;
        isSoilWet3 = false;

        // Log event for low soil moisture
        Blynk.logEvent("dry_soil_notification", "Soil Moisture 3 Low! Pump starting...");
    } else if (soilMoisturePercent3 > soilMoistureThresholdMAX && !isSoilWet3) {
        digitalWrite(relayPin4, LOW);
        Blynk.virtualWrite(V6, 0);
        isSoilWet3 = true;
        isSoilDry3 = false;

        // Log event for soil moisturized
        Blynk.logEvent("wet_soil_notification", "Plant soil 3 moisturized! Pump turned off...");
    }
}
