#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SDA_PIN 4
#define SCL_PIN 15

#define GPS_RX 17
#define GPS_TX 33

const char* NODE_ID = "NODE_1";

TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Starting LoRa...");
  display.display();

  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("LoRa Init Failed");
    display.display();
    while (1);
  }

  Serial.println("LoRa Transmitter");

  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("LoRa Initialized");
  display.display();

  gpsSerial.begin(9600);
  display.setCursor(0, 20);
  display.print("GPS Initialized");
  display.display();
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());

    if (gps.location.isUpdated()) {
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();

      Serial.print("Node: ");
      Serial.print(NODE_ID);
      Serial.print(" | Lat: ");
      Serial.print(latitude, 6);
      Serial.print(", Lon: ");
      Serial.println(longitude, 6);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Node: ");
      display.println(NODE_ID);
      display.print("Lat: ");
      display.println(latitude, 6);
      display.print("Lon: ");
      display.println(longitude, 6);

      String gpsData = String(NODE_ID) + "," + String(latitude, 6) + "," + String(longitude, 6);
      LoRa.beginPacket();
      LoRa.print(gpsData);
      LoRa.endPacket();
    }
  }
}
