# STM32 NRF24L01 Transmitter & PPM Receiver

## 📌 Overview
This project consists of a **wireless transmitter and receiver system** using an **STM32 microcontroller**, an **NRF24L01 module**, and **PPM output** for controlling an FC (Flight Controller). The transmitter reads joystick inputs, sends them wirelessly using NRF24L01, and the receiver converts the received data into PPM signals for the flight controller.

## 🛠 Components Used
### **Transmitter (TX)**
- **STM32 Black Pill (STM32F411)**
- **NRF24L01+ Module**
- **Joystick (Analog)**
- **SSD1306 OLED Display**
- **Push Buttons (For Auxiliary Channels)**

### **Receiver (RX)**
- **STM32 Black Pill (STM32F411)**
- **NRF24L01+ Module**
- **PPM Output to Flight Controller (FC)**

---
## 🔧 **Transmitter Code Explanation**
### **1️⃣ Read Analog Inputs**
The STM32 reads joystick values from analog pins:
```cpp
// Read ADC values
 data.throttle = analogRead(PA2);
 data.yaw      = analogRead(PA3);
 data.pitch    = analogRead(PA1);
 data.roll     = analogRead(PA0);
 data.aux1     = analogRead(PA4);
 data.aux2     = analogRead(PA5);
 data.aux3     = analogRead(PA6);
 data.aux4     = analogRead(PA7);
```

### **2️⃣ Adjust ADC Scaling for 2.9V Max Input**
Since the joystick max voltage is **2.9V** instead of **3.3V**, we scale the values:
```cpp
// Normalize ADC readings for correct mapping
data.throttle = map(analogRead(PA2), 0, 3598, 0, 4095);
data.yaw      = map(analogRead(PA3), 0, 3598, 0, 4095);
data.pitch    = map(analogRead(PA1), 0, 3598, 0, 4095);
data.roll     = map(analogRead(PA0), 0, 3598, 0, 4095);
```

### **3️⃣ Transmit Data Using NRF24L01**
The data is sent over **NRF24L01+** to the receiver:
```cpp
bool success = radio.write(&data, sizeof(DataPacket));
if (success) {
    Serial.println("✅ Data Sent Successfully.");
} else {
    Serial.println("❌ Data Transmission Failed.");
}
```

---
## 🔧 **Receiver Code Explanation**
### **1️⃣ Receive Data From TX**
```cpp
if (radio.available()) {
    radio.read(&data, sizeof(DataPacket));
}
```

### **2️⃣ Convert Data to PPM Output**
```cpp
ppmChannels[0] = map(data.throttle, 0, 3598, 1000, 2000);
ppmChannels[1] = map(data.yaw, 0, 3598, 1000, 2000);
ppmChannels[2] = map(data.pitch, 0, 3598, 1000, 2000);
ppmChannels[3] = map(data.roll, 0, 3598, 1000, 2000);
```
This ensures the received joystick values are mapped to **PPM signals** (1000-2000μs range) for the Flight Controller.

---
## 📌 **Installation & Setup**
### **1️⃣ Wiring Connections**
#### **Transmitter (TX)**
| STM32 Black Pill | NRF24L01 |
|------------------|----------|
| 3.3V            | VCC      |
| GND             | GND      |
| PB15            | MOSI     |
| PB14            | MISO     |
| PB13            | SCK      |
| PB12            | CSN      |
| PB1             | CE       |

#### **Receiver (RX)**
| STM32 Black Pill | NRF24L01 |
|------------------|----------|
| 3.3V            | VCC      |
| GND             | GND      |
| PB15            | MOSI     |
| PB14            | MISO     |
| PB13            | SCK      |
| PB12            | CSN      |
| PB1             | CE       |

### **2️⃣ Install Required Libraries**
Ensure you have these libraries installed in the Arduino IDE:
- `RF24 by TMRh20`
- `Adafruit GFX`
- `Adafruit SSD1306`

### **3️⃣ Upload Code to STM32**
1. Install **STM32CubeProgrammer** and **STM32duino Bootloader**.
2. Connect STM32 via **USB TTL Adapter**.
3. Flash the TX and RX firmware.

---
## 🎯 **Project Features**
✅ **PPM Output for Flight Controller**
✅ **Wireless Communication using NRF24L01**
✅ **OLED Display for Debugging**
✅ **Customizable Joystick Mapping**
✅ **Supports Auxiliary Channels (AUX1-AUX4)**

---
## 📌 **Future Improvements**
- ✅ Add **Failsafe Mode** in case of signal loss
- ✅ Implement **Telemetry Feedback** for battery and signal strength
- ✅ Optimize **PPM Signal Timing**

---
## 📞 **Support & Contact**
For any issues, reach out via **GitHub Issues** or contact me at **your.email@example.com** 🚀

