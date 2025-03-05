#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define SPI2 for STM32 (PB15, PB14, PB13)
SPIClass SPI_2(PB15, PB14, PB13);

// NRF24L01 Pin Definitions
#define CE_PIN  PB1
#define CSN_PIN PB12

// PPM Output Pin
#define PPM_OUT PB0  // Connect this to FC PPM input

// PPM Constants
#define PPM_FRAME_LENGTH 22500  // 22.5ms (50Hz Frame)
#define PPM_PULSE_LENGTH 300    // 300µs pulse length

// NRF24L01 Object
RF24 radio(CE_PIN, CSN_PIN);

struct DataPacket {
  int throttle, yaw, pitch, roll, aux1, aux2, aux3, aux4;
} data;

// PPM Channel Data (Default Values)
uint16_t ppmChannels[8] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

// Interrupt Routine to Generate PPM
void TIM1_UP_TIM10_IRQHandler(void) {
  static uint8_t channel = 0;
  static uint32_t lastTime = 0;
  uint32_t currentTime = micros();
  uint32_t elapsedTime = currentTime - lastTime;
  lastTime = currentTime;

  if (channel < 8) {
    // Set HIGH for pulse
    digitalWrite(PPM_OUT, HIGH);
    delayMicroseconds(PPM_PULSE_LENGTH);
    digitalWrite(PPM_OUT, LOW);
    
    // Delay for channel value
    delayMicroseconds(ppmChannels[channel] - PPM_PULSE_LENGTH);
    channel++;
  } else {
    // End of frame, reset
    delayMicroseconds(PPM_FRAME_LENGTH - elapsedTime);
    channel = 0;
  }

  TIM1->SR &= ~TIM_SR_UIF;  // Clear interrupt flag
}

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Receiver Initializing...");

  // Enable SPI2
  Serial.println("🔄 Enabling SPI2...");
  SPI_2.begin();

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
  radio.openReadingPipe(0, 0xE8E8F0F0E1LL);
  radio.startListening();

  // Setup PPM Output
  pinMode(PPM_OUT, OUTPUT);
  digitalWrite(PPM_OUT, LOW);

  // Setup Timer1 for PPM (TIM1)
  TIM1->PSC = 84 - 1;  // Prescaler (1µs resolution)
  TIM1->ARR = PPM_FRAME_LENGTH;  // Set frame length
  TIM1->DIER |= TIM_DIER_UIE;  // Enable update interrupt
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);  // Enable TIM1 interrupt
  TIM1->CR1 |= TIM_CR1_CEN;  // Start timer
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(DataPacket));

    // Convert Received Data to PPM Format
    ppmChannels[0] = map(data.throttle, 0, 3598, 1000, 2000);
    ppmChannels[1] = map(data.yaw, 0, 3598, 1000, 2000);
    ppmChannels[2] = map(data.pitch, 0, 3598, 1000, 2000);
    ppmChannels[3] = map(data.roll, 0, 3598, 1000, 2000);
    ppmChannels[4] = map(data.aux1, 0, 4095, 1000, 2000);
    ppmChannels[5] = map(data.aux2, 0, 4095, 1000, 2000);
    ppmChannels[6] = map(data.aux3, 0, 4095, 1000, 2000);
    ppmChannels[7] = map(data.aux4, 0, 4095, 1000, 2000);

    Serial.print("📩 PPM Output - ");
    for (int i = 0; i < 8; i++) {
      Serial.print("CH"); Serial.print(i+1);
      Serial.print(":"); Serial.print(ppmChannels[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}
