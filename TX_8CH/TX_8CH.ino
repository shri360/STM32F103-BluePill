#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define throttle_pin PA2
#define yaw_pin PA3
#define roll_pin PA0
#define pitch_pin PA1
#define aux1_pin PB8
#define aux2_pin PB3
#define aux3_pin PB9
#define aux4_pin PB10

const uint64_t pipeOut = 0xE8E8F0F0E1LL;
RF24 radio(PB4, PB5);  // CSN, CE

uint8_t sbusPacket[25];
uint16_t channels[16];  // SBUS 16 channels

void resetChannels() {
  for (int i = 0; i < 16; i++) channels[i] = 1024;
}

void encodeSBUS() {
  memset(sbusPacket, 0, 25);
  sbusPacket[0] = 0x0F;

  uint32_t temp = 0;
  int byteIndex = 1;
  int bitOffset = 0;

  for (int i = 0; i < 16; i++) {
    temp |= ((uint32_t)(channels[i] & 0x07FF)) << bitOffset;
    bitOffset += 11;
    while (bitOffset >= 8) {
      sbusPacket[byteIndex++] = temp & 0xFF;
      temp >>= 8;
      bitOffset -= 8;
    }
  }

  sbusPacket[23] = 0x00;  // Flags byte (no failsafe)
  sbusPacket[24] = 0x00;  // End byte
}

void setup() {
  pinMode(throttle_pin, INPUT);
  pinMode(yaw_pin, INPUT);
  pinMode(roll_pin, INPUT);
  pinMode(pitch_pin, INPUT);
  pinMode(aux1_pin, INPUT_PULLUP);
  pinMode(aux2_pin, INPUT_PULLUP);
  pinMode(aux3_pin, INPUT_PULLUP);  // NEW
  pinMode(aux4_pin, INPUT_PULLUP);  // NEW

  Serial.begin(115200);

  radio.begin();
  radio.setAutoAck(false);
  radio.setChannel(0x2F);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(pipeOut);

  resetChannels();
  Serial.println("TX ready");
}

void loop() {
  // Read analog inputs
  uint16_t rawThrottle = analogRead(throttle_pin);
  uint16_t rawYaw = analogRead(yaw_pin);
  uint16_t rawRoll = analogRead(roll_pin);
  uint16_t rawPitch = analogRead(pitch_pin);

  // Read digital switches
  bool rawAux1 = !digitalRead(aux1_pin);
  bool rawAux2 = !digitalRead(aux2_pin);
  bool rawAux3 = !digitalRead(aux3_pin);  // NEW
  bool rawAux4 = !digitalRead(aux4_pin);  // NEW

  // Map to SBUS range (172–1811)
  channels[0] = map(rawThrottle, 587, 404, 210, 1850);
  channels[1] = map(rawYaw, 450, 638, 172, 1811);
  channels[2] = map(rawRoll, 420, 620, 172, 1811);
  channels[3] = map(rawPitch, 460, 630, 172, 1811);
  channels[4] = rawAux1 ? 1811 : 172;
  channels[5] = rawAux2 ? 1811 : 172;
  channels[6] = rawAux3 ? 1811 : 172;  // CH7
  channels[7] = rawAux4 ? 1811 : 172;  // CH8

  // Other channels mid value
  for (int i = 8; i < 16; i++) channels[i] = 1024;

  // Encode and transmit
  encodeSBUS();
  radio.write(&sbusPacket, 25);

  delay(14);  // ~70Hz frame rate
}
