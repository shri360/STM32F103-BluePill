#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Use same pipe address as transmitter
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

// CE, CSN pins (same as your TX: CE=PB5, CSN=PB4)
RF24 radio(PB5, PB4);

// Struct must match TX side
struct MyData {
  long throttle;
  long yaw;
  long roll;
  long pitch;
  bool AUX1;
  bool AUX2;
};

MyData receivedData;

void setup() {
  Serial.begin(9600);
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
    radio.read(&receivedData, sizeof(MyData));

    Serial.println("==== Received Data ====");
    Serial.print("Throttle: "); Serial.println(receivedData.throttle);
    Serial.print("Yaw     : "); Serial.println(receivedData.yaw);
    Serial.print("Roll    : "); Serial.println(receivedData.roll);
    Serial.print("Pitch   : "); Serial.println(receivedData.pitch);
    Serial.print("AUX1    : "); Serial.println(receivedData.AUX1);
    Serial.print("AUX2    : "); Serial.println(receivedData.AUX2);
    Serial.println("========================");
  }
}
