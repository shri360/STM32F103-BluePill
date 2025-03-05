#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define SPI2 for STM32 (PB15, PB14, PB13)
SPIClass SPI_2(PB15, PB14, PB13);

// NRF24L01 Pin Definitions
#define CE_PIN  PB1
#define CSN_PIN PB12

// OLED Display Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// NRF24L01 Object
RF24 radio(CE_PIN, CSN_PIN);

// Data Structure
struct DataPacket {
  int throttle, yaw, pitch, roll, aux1, aux2, aux3, aux4;
} data;

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Transmitter Initializing...");

  // Enable SPI2
  Serial.println("🔄 Enabling SPI2...");
  SPI_2.begin();

  // Set CE & CSN as Output
  pinMode(CE_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);

  // Initialize NRF24L01 with SPI2
  Serial.println("🔄 Initializing NRF24L01...");
  if (!radio.begin(&SPI_2)) {
    Serial.println("❌ NRF24L01 Initialization Failed!");
    while (1);
  }
  Serial.println("✅ NRF24L01 Initialized Successfully.");

  // NRF24L01 Settings
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(0xE8E8F0F0E1LL);
  radio.stopListening();

  // Initialize OLED Display
  Serial.println("🔄 Initializing OLED...");
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED Display Initialization Failed!");
    while (1);
  }
  Serial.println("✅ OLED Display Initialized.");
  display.clearDisplay();
  display.display();
}

void loop() {
  // Read Analog Inputs
  data.throttle = analogRead(PA2);
  data.yaw = analogRead(PA3);
  data.pitch = analogRead(PA1);
  data.roll = analogRead(PA0);
  data.aux1 = analogRead(PA4);
  data.aux2 = analogRead(PA5);
  data.aux3 = analogRead(PA6);
  data.aux4 = analogRead(PA7);

  // Debugging Output
  Serial.print("📡 Sending Data - ");
  Serial.print("T:"); Serial.print(data.throttle);
  Serial.print(" Y:"); Serial.print(data.yaw);
  Serial.print(" P:"); Serial.print(data.pitch);
  Serial.print(" R:"); Serial.print(data.roll);
  Serial.print(" A1:"); Serial.print(data.aux1);
  Serial.print(" A2:"); Serial.print(data.aux2);
  Serial.print(" A3:"); Serial.print(data.aux3);
  Serial.print(" A4:"); Serial.println(data.aux4);

  // Transmit Data
  bool success = radio.write(&data, sizeof(DataPacket));
  if (success) {
    Serial.println("✅ Data Sent Successfully.");
  } else {
    Serial.println("❌ Data Transmission Failed.");
  }

  // Update OLED Display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("T:"); display.print(data.throttle);
  display.print(" Y:"); display.print(data.yaw);
  display.print(" P:"); display.print(data.pitch);
  display.print(" R:"); display.print(data.roll);
  display.display();
  Serial.println("🖥 OLED Updated.");

  delay(10);
}
