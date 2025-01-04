#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI0 26
#define BAND 868E6

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SDA_PIN 4
#define SCL_PIN 15

const char* ssid = "S.Pokhrel";
const char* password = "9876543211";
const char* serverURL = "http://172.20.10.3:5001/receive-data";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    while (1);
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("LoRa Initialized");
  display.display();

  connectToWiFi();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    Serial.println("Received: " + receivedData);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Received Data:");
    display.setCursor(0, 10);
    display.print(receivedData);
    display.display();

    sendDataToServer(receivedData);
  }
}

void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("WiFi Connected");
  display.display();
}

void sendDataToServer(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String fullURL = String(serverURL) + "?data=" + data;

    http.begin(fullURL);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected.");
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("WiFi Not Connected");
    display.display();
  }
}
