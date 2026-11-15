#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Settings (128x64 I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
const int soilPin = A0;     // Soil moisture sensor analog pin
const int redLedPin = 7;    // Red LED (Needs Water)
const int greenLedPin = 8;  // Green LED (Soil is Good)

// Calibration Values (Adjust based on your sensor in dry air vs. water)
const int dryValue = 1023;  // Raw analog value in completely dry soil/air
const int wetValue = 300;   // Raw analog value in wet soil/water

// Moisture Threshold (%)
const int moistureThreshold = 40; // Below 40% = Needs Water

void setup() {
  Serial.begin(9600);

  // Configure LED pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);

  // Initialize OLED Display (default I2C address 0x3C)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 OLED allocation failed"));
    for (;;); // Stop if OLED is not detected
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 25);
  display.println(F("Smart Plant Monitor"));
  display.display();
  delay(2000);
}

void loop() {
  // Read raw analog value and convert to percentage (0% - 100%)
  int rawValue = analogRead(soilPin);
  int moisturePercent = map(rawValue, dryValue, wetValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // Print readings to Serial Monitor for debugging
  Serial.print("Raw Value: ");
  Serial.print(rawValue);
  Serial.print(" | Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // Control Red and Green LEDs based on threshold
  bool needsWater = (moisturePercent < moistureThreshold);

  if (needsWater) {
    digitalWrite(redLedPin, HIGH);    // Turn ON Red LED
    digitalWrite(greenLedPin, LOW);   // Turn OFF Green LED
  } else {
    digitalWrite(redLedPin, LOW);     // Turn OFF Red LED
    digitalWrite(greenLedPin, HIGH);  // Turn ON Green LED
  }

  // Update OLED Display
  display.clearDisplay();

  // Header
  display.setTextSize(1);
  display.setCursor(12, 0);
  display.println(F("PLANT HEALTH MONITOR"));
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // Moisture Rate Percentage
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print(F("Soil: "));
  display.print(moisturePercent);
  display.println(F("%"));

  // Status Message
  display.setTextSize(1);
  display.setCursor(10, 48);
  if (needsWater) {
    display.println(F("Status: NEEDS WATER!"));
  } else {
    display.println(F("Status: SOIL IS GOOD"));
  }

  display.display();
  delay(1000); // Update every 1 second
}
