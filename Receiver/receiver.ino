#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Heltec LoRa32 Pinouts
#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DI0     26
#define BAND    868E6  // Set your frequency

// OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    16  // Reset pin is 16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// SDA and SCL Pins for OLED I2C
#define SDA_PIN 4
#define SCL_PIN 15

// WiFi credentials
const char* ssid = "S.Pokhrel";
const char* password = "9876543211";

// Flask server URL
const char* serverURL = "http://172.20.10.3:5001/receive-data";

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);
  while (!Serial);

  // Initialize I2C communication for the OLED with custom SDA and SCL pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize LoRa
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("LoRa Init Failed");
    display.display();
    while (1);
  }

  Serial.println("LoRa Receiver");

  // Display success on OLED
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("LoRa Initialized");
  display.display();

  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Check if a packet is available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    // Print received data to serial monitor
    Serial.println("Received: " + receivedData);

    // Display received data on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Received Data:");
    display.setCursor(0, 10);
    display.print(receivedData);
    display.display();

    // Send received data to Flask server
    sendDataToServer(receivedData);
  }
}

// Function to connect to WiFi
void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Connecting to WiFi...");
  display.display();

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("WiFi Connected");
  display.display();
}

// Function to send data to the Flask server
void sendDataToServer(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String fullURL = String(serverURL) + "?data=" + data;

    http.begin(fullURL);  // Specify destination
    int httpResponseCode = http.GET();  // Send HTTP GET request

    if (httpResponseCode > 0) {
      String response = http.getString();  // Get the response
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("Error in sending request: " + String(httpResponseCode));
    }

    http.end();  // Free resources
  } else {
    Serial.println("WiFi not connected.");
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("WiFi Not Connected");
    display.display();
  }
}
