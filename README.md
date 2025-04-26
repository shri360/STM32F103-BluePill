
Project: SBUS over NRF24L01+ Transmission for INAV / Betaflight  using GD32F103C8T6 (STM32 Bluepill Clone)
Date: 2025-04-26

Description:
------------
This project implements wireless SBUS transmission using NRF24L01+ modules
and GD32F103 (or STM32F103) microcontrollers.

- The TX side reads analog and digital signals (joystick and switches),
  converts them into SBUS format, and sends them wirelessly via NRF24L01+.
- The RX side receives the SBUS packets and outputs standard SBUS signal
  through UART (for use with INAV, Betaflight, or any SBUS receiver input).

Hardware Connections:
----------------------

TX SIDE:
- CE  -> PB5
- CSN -> PB4
- SPI -> SPI1
- Analog inputs:
    - Throttle -> PA2
    - Yaw      -> PA3
    - Roll     -> PA0
    - Pitch    -> PA1
- Switch inputs (Digital with pull-up):
    - AUX1 -> PB8
    - AUX2 -> PB3

RX SIDE:
- CE  -> PB5
- CSN -> PB4
- SPI -> SPI1
- SBUS Output via USART2 TX (PA2)
- Debugging via USB Serial (Serial / USART1)

Important Notes:
----------------

1. NRF24L01+ modules require stable 3.3V power!
   - Add 10µF–100µF capacitor between VCC and GND pins near NRF module.
   - If using high-power modules (PA+LNA), use a good 3.3V regulator.

2. Voltage brownout during motor startup can reset the RX board.
   - Add a 470µF (or larger) capacitor between 3.3V and GND on RX side.
   - Ensure stable 5V supply to the board if stepping down to 3.3V.

3. SBUS Frame Details:
   - 25 bytes per frame.
   - Baudrate 100000bps, 8 data bits, Even parity, 2 stop bits (8E2).

4. Calibration of Channels:
   - Analog channels are mapped into SBUS range (172–1811).
   - Adjust mapping ranges in TX code if needed based on joystick values.

5. Flags:
   - SBUS flags (byte 23) can set "failsafe" and "frame lost" if needed.

Compilation:
------------

Use Arduino IDE or PlatformIO with STM32duino or GD32 Arduino Core.

Board: "Generic STM32F103C8" or "GD32F103C8"
Upload Method: STM32duino bootloader / Serial bootloader

Dependencies:
-------------
- RF24 Arduino library
  (https://github.com/nRF24/RF24)

- STM32duino (or GD32 Arduino Core)

How to Use:
-----------

1. Flash the TX code to the transmitter board.
2. Flash the RX code to the receiver board.
3. Connect the RX SBUS output to flight controller SBUS input.
4. Configure INAV / Betaflight receiver settings to "Serial RX" -> "SBUS".
5. Adjust AUX switches and arming ranges inside configurator if necessary.
6. Add capacitors if RX resets during motor spin-up.

Troubleshooting:
----------------

- "Receiver Ready" printed multiple times => RX rebooting => Add capacitors!
- SBUS values not reaching FC => check UART configuration and SBUS polarity.
- High packet loss => Lower NRF24L01+ data rate to 1Mbps if necessary.

License:
--------
MIT License. Free to use and modify.

Credits:
--------
- Based on standard SBUS protocol documentation.
- NRF24 communication based on TMRh20's RF24 library.

