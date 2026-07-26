# AI-Powered Tactile Smart Watch for Deaf, Hard-of-Hearing & Speech-Impaired Users

Welcome to the project repository for the **Tactile Smart Watch and Companion Mobile App System**.

This project provides real-time environmental sound recognition (sirens, doorbells, fire alarms, baby crying), dual-mic sound direction detection (Left vs Right), two-way speech communication (100% Free On-Device Speech-to-Text & Text-to-Speech), and emergency location-based SOS text dispatch.

---

## 📂 Project Documentation Structure

The documentation is cleanly organized into 5 dedicated, easy-to-understand modules:

1. **[01_COMPLETE_BILL_OF_MATERIALS_INDIA.md](file:///home/jeevan/Desktop/my%20projects/smart-watch/01_COMPLETE_BILL_OF_MATERIALS_INDIA.md)**  
   *Exhaustive Indian shopping list including main modules AND all minor ₹1 to ₹10 items (resistors, caps, jumper wires, breadboard, switches, JST connectors) with direct Robu.in purchase links.*

2. **[02_SYSTEM_ARCHITECTURE_AND_LOGIC.md](file:///home/jeevan/Desktop/my%20projects/smart-watch/02_SYSTEM_ARCHITECTURE_AND_LOGIC.md)**  
   *Complete system architecture, FreeRTOS dual-core allocation, sound direction DSP math (TDOA/ILD), BLE ADPCM compression, and concurrency auto-pause rules.*

3. **[03_HARDWARE_WIRING_AND_PINMAP.md](file:///home/jeevan/Desktop/my%20projects/smart-watch/03_HARDWARE_WIRING_AND_PINMAP.md)**  
   *Complete pin connection table, breadboard wiring guide, mechanical layer stack, and acoustic/thermal isolation rules.*

4. **[04_TINYML_AND_FIRMWARE_GUIDE.md](file:///home/jeevan/Desktop/my%20projects/smart-watch/04_TINYML_AND_FIRMWARE_GUIDE.md)**  
   *Step-by-step Edge Impulse audio dataset collection, MFE training, C++ library export, and complete ESP32-S3 firmware code (`main.cpp`).*

5. **[05_MOBILE_APP_SPECIFICATION.md](file:///home/jeevan/Desktop/my%20projects/smart-watch/05_MOBILE_APP_SPECIFICATION.md)**  
   *React Native companion mobile app architecture, free native STT/TTS integration, BLE manager, and direct SIM SMS emergency SOS dispatch.*

---

## ⚡ Quick Start Options

* **Path 1 (Recommended All-in-One Path ~₹2,645 Total):** Buy the Waveshare ESP32-S3-Touch-LCD-1.28 round board + 4 sensors/actuators. No complex display soldering required!
* **Path 2 (Modular Breadboard Path ~₹1,850 Total):** Buy individual ESP32-S3 Mini, round LCD module, and breakout boards.
