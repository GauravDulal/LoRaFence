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

#define GPS_RX 33
#define GPS_TX 17

#define BUZZER_PIN 23
#define BUZZER_PIN2 13

const char* NODE_ID = "NODE_1";

TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

struct Point {
  double lat;
  double lng;
};

const Point innerGeofence[] = {
  {27.688771, 85.328188},
  {27.688981, 85.327117},
  {27.689651, 85.327594},
  {27.689654, 85.328408}
};
const int innerNumVertices = sizeof(innerGeofence) / sizeof(innerGeofence[0]);

const Point outerGeofence[] = {
  {27.688800, 85.328268},
  {27.689017, 85.327027},
  {27.689680, 85.327534},
  {27.689683, 85.328488}
};
const int outerNumVertices = sizeof(outerGeofence) / sizeof(outerGeofence[0]);

bool isInsidePolygon(const Point* polygon, int numVertices, double lat, double lng) {
  bool inside = false;
  for (int i = 0, j = numVertices - 1; i < numVertices; j = i++) {
    if (((polygon[i].lng > lng) != (polygon[j].lng > lng)) &&
        (lat < (polygon[j].lat - polygon[i].lat) * (lng - polygon[i].lng) / (polygon[j].lng - polygon[i].lng) + polygon[i].lat)) {
      inside = !inside;
    }
  }
  return inside;
}

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

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUZZER_PIN2, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(BUZZER_PIN2, LOW);
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

      bool insideInner = isInsidePolygon(innerGeofence, innerNumVertices, latitude, longitude);
      bool insideOuter = isInsidePolygon(outerGeofence, outerNumVertices, latitude, longitude);

      if (!insideOuter) {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(BUZZER_PIN2, HIGH);
        Serial.println("Outside outer geofence! Both buzzers ON.");

        display.setCursor(0, 40);
        display.print("Outside Outer Geofence!");
        display.display();
      } else if (!insideInner) {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(BUZZER_PIN2, LOW);
        Serial.println("Outside inner geofence! Buzzer 1 ON.");

        display.setCursor(0, 40);
        display.print("Outside Inner Geofence!");
        display.display();
      } else {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(BUZZER_PIN2, LOW);
        Serial.println("Inside both geofences.");

        display.setCursor(0, 40);
        display.print("Inside Geofences");
        display.display();
      }
    }
  }
}
