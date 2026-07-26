# Module 03: Hardware Wiring, Pin Map & Mechanical Assembly

## 1. Master ESP32-S3 Pin Mapping Table

| Component Module | Module Pin | ESP32-S3 Pin | Protocol / Signal | Wiring Notes |
| :--- | :--- | :--- | :--- | :--- |
| **Mic Left (INMP441)** | `SCK` | **GPIO 14** | I2S Clock | Shared I2S Clock bus |
| | `WS` | **GPIO 15** | I2S Word Select | Shared Word Select bus |
| | `SD` | **GPIO 32** | I2S Data In | Left channel data |
| | `L/R` | **GND** | Channel Select | GND sets Left channel |
| | `VDD` / `GND` | 3.3V / GND | Power | Place 100nF cap across VDD/GND |
| **Mic Right (INMP441)**| `SCK` | **GPIO 14** | I2S Clock | Shared I2S Clock bus |
| | `WS` | **GPIO 15** | I2S Word Select | Shared Word Select bus |
| | `SD` | **GPIO 32** (or 33)| I2S Data In | Right channel data (L/R to 3.3V) |
| | `L/R` | **3.3V** | Channel Select | 3.3V sets Right channel |
| **DRV2605L Haptics** | `SDA` | **GPIO 21** | I2C Data | 4.7kΩ pull-up to 3.3V |
| | `SCL` | **GPIO 22** | I2C Clock | 4.7kΩ pull-up to 3.3V |
| | `IN/TRIG` | **GND** | Mode Select | I2C internal trigger mode |
| | `OUT+` / `OUT-`| LRA Motor Pins | Motor Drive | Connect directly to LRA coin motor |
| **MAX98357A Amp** | `BCLK` | **GPIO 27** | I2S1 Bit Clock | Independent I2S Audio Out bus |
| | `LRC` | **GPIO 26** | I2S1 Left/Right | Frame Clock |
| | `DIN` | **GPIO 25** | I2S1 Data Out | Audio PCM Data |
| | `GAIN` / `SD` | 3.3V / Unconnected| Gain Setting | 3.3V sets 12dB gain |
| | `SPEAKER +/-` | 8Ω Micro Speaker | Audio Output | Connect 0.5W micro speaker |
| **GC9A01 LCD (SPI)** | `MOSI` | **GPIO 11** | SPI Data Out | Main display data line |
| | `SCLK` | **GPIO 12** | SPI Clock | Display clock line |
| | `CS` | **GPIO 10** | SPI Chip Select | Display select |
| | `DC` | **GPIO 9** | Data/Command | Command toggle pin |
| | `RST` | **GPIO 13** | Hardware Reset | Display reset pin |
| | `BL` | **GPIO 18** | Backlight Control | PWM brightness or 3.3V |
| **SOS Push Button** | Pin 1 / Pin 2 | **GPIO 4** / GND | Digital Interrupt | 10kΩ pull-up resistor to 3.3V |
| **Power & Battery** | `VBUS` / `BAT` | USB 5V / LiPo + | Power Path | Connect 3.7V LiPo via slide switch |

---

## 2. Circuit Protection & Decoupling Guide

To prevent microphone noise and false TinyML triggers during haptic motor activation:

1. **Microphone Noise Filter:** Solder a **100nF (0.1µF) ceramic capacitor** directly across the `VDD` and `GND` pins of each INMP441 microphone module.
2. **I2C Pull-Up Resistors:** Connect **4.7kΩ resistors** from GPIO 21 (`SDA`) to 3.3V and GPIO 22 (`SCL`) to 3.3V.
3. **SOS Button Debounce:** Connect a **10kΩ resistor** from GPIO 4 to 3.3V and add a **100nF capacitor** in parallel with the SOS button pins to prevent mechanical switch bounce.

---

## 3. Mechanical Stack & Insulation Rules

```text
+-------------------------------------------------------------------+  <-- Top Case / Bezel
|                      1.28" GC9A01 Round LCD                       |
+-------------------------------------------------------------------+
| Side Case Gaskets | Dual Mic Ports (Silicone Rubber Isolation Boots)|
+-------------------------------------------------------------------+
|               Main PCB: ESP32-S3 + DRV2605L + MAX98357A          |
+-------------------------------------------------------------------+
| ===== THERMAL BARRIER LAYER (0.5mm FR4 / Kapton Insulation) ===== |
+-------------------------------------------------------------------+
|                    3.7V 600mAh LiPo Battery                       |
+-------------------------------------------------------------------+
|              Bottom Case Plate: LRA Haptic Motor                 |
+-------------------------------------------------------------------+  <-- Wrist Contact
```

### Acoustic Isolation Rule:
The dual microphones **must be encased in soft silicone rubber boots** separating the acoustic port from the 3D printed plastic case. This stops the motor vibration from traveling through the plastic housing into the microphone sensors.
