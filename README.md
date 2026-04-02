# Keero Bot – Firmware

<p align="center">
  <img src="../keero-hardware/assets/hero.png" alt="Keero Bot Firmware" width="100%" />
</p>

<p align="center">
  <img src="https://img.shields.io/badge/status-in%20development-orange" alt="Status" />
  <img src="https://img.shields.io/badge/firmware-open--source-blue" alt="Open Source Firmware" />
  <img src="https://img.shields.io/badge/platform-ESP32--S3-green" alt="ESP32-S3" />
</p>

---

The Keero Bot firmware powers the hardware platform by handling communication, interaction, and AI-driven behavior.

It is designed to run on the ESP32-S3 and act as the central logic layer connecting all hardware components.

---

## 🚀 Overview

The firmware is responsible for:

- Device control and hardware abstraction  
- Sensor data processing  
- Audio input/output handling  
- Display rendering  
- Communication with external services  
- AI interaction logic  

It is built with a modular structure to support future expansion and additional hardware modules.

---

## 🧠 Core Features

### ⚙️ Hardware Control
- GPIO, I2C, SPI, UART handling  
- Camera control (OV2640)  
- Display control (OLED)  
- Power management integration (PMIC)

### 🔊 Audio System
- I2S microphone input  
- Audio output via amplifier  
- Real-time audio handling  

### 📳 Interaction
- Haptic feedback control  
- Button handling  
- Motion-based triggers (accelerometer)

### 📡 Connectivity
- WiFi / BLE communication  
- External module communication (UART via pogo pins)

### 🤖 AI Layer (Planned / In Progress)
- State-driven behavior system  
- Emotion/state engine  
- Integration with AI services (ChatGPT, etc.)
- Remote messaging (server / Telegram)

---

## 📂 Repository Structure

```text
keero-firmware/
├── src/
├── include/
├── lib/
├── configs/
├── scripts/
├── docs/
└── platformio.ini
````

---

## ⚙️ Setup

### Requirements

* ESP32-S3 board
* PlatformIO (recommended) or Arduino IDE

### Flashing

1. Connect device via USB
2. Build firmware
3. Upload to device

(Full instructions will be added in future updates)

---

## 🧪 Status

* Core firmware: 🚧 In development
* Hardware integration: 🚧 In progress
* AI features: 🚧 Planned

---

## 🎯 Goals

* Modular and scalable firmware architecture
* Tight integration with hardware platform
* Real-time interaction capabilities
* AI-driven behavior and communication

---

## ⚠️ License

This firmware is open-source but **not intended for unrestricted commercial use**.

Commercial usage requires explicit permission.

---

## 🌐 About Keero

Keero is focused on building experimental AI-driven hardware platforms that combine embedded systems with real-world interaction.

````

---

# 🔥 Ako želiš još jače (preporučam)

Sljedeći upgrade koji ti diže projekt:

👉 dodati sekciju:
```md
## 🧠 Architecture
````

gdje objasniš:

* event loop
* state machine
* taskovi

