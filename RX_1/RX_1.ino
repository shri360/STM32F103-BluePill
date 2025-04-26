#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Declare Serial2 manually
HardwareSerial Serial2(PA3, PA2);  // RX, TX for USART2

const uint64_t pipeIn = 0xE8E8F0F0E1LL;
RF24 radio(PB5, PB4);  // CE, CSN (match TX side)

uint8_t sbusPacket[25];

void setup() {
  Serial.begin(9600);   // USB serial for debugging
  Serial2.begin(100000, SERIAL_8E2);  // SBUS settings

  delay(1000);

  radio.begin();
  radio.setAutoAck(false);
  radio.setChannel(0x2F);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();

  Serial.println("Receiver Ready");
}

void loop() {
  if (radio.available()) {
    radio.read(&sbusPacket, 25);

    // Always clear failsafe bits here if needed (optional)
    sbusPacket[24] &= ~(1 << 2);  // Clear Failsafe bit
    sbusPacket[24] &= ~(1 << 3);  // Clear Frame Lost bit

    Serial2.write(sbusPacket, 25);  // Output SBUS
  }
}
