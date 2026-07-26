# Module 06: Existing Projects & GitHub Innovation Comparison

This module documents existing open-source research projects on GitHub, compares them with this **ESP32-S3 Tactile Smart Watch**, and highlights the core novel innovations of this project.

---

## 🔍 1. Existing Open-Source Projects on GitHub

| Repository / Project | Organization / Creator | Technology Stack | Key Features | Limitations |
| :--- | :--- | :--- | :--- | :--- |
| **[SoundWatch](https://github.com/makeabilitylab/SoundWatch)** | Univ. of Washington (Makeability Lab) | Android WearOS + Deep Learning (YAMNet) | Real-time sound awareness & haptic alerts on commercial smartwatches | ❌ Requires expensive $200+ WearOS smartwatch; cannot run on standalone microcontrollers |
| **[AudioCompass](https://github.com/mikalhart/AudioCompass)** | Open-Source Arduino Library | ESP32-S3 + I2S Mics + TinyML | Sound direction estimation (Front, Back, Left, Right) using 2 mics | ❌ Only a raw library; lacks display UI, haptics, BLE companion app, and SMS dispatch |
| **[Sonavi](https://github.com/xyugen/sonavi)** | Independent Researcher | WearOS Watch + Android App (TFLite) | Offline environmental sound recognition and haptic alerts | ❌ Depends on WearOS platform; no direction detection or 2-way speech |
| **[ProtoSound](https://github.com/makeabilitylab/ProtoSound)** | Univ. of Washington | Mobile App + Edge ML | User-personalized real-time sound training system | ❌ Mobile-centric; no dedicated low-cost wearable hardware |
| **[Vibes](https://github.com/higherdefender/Vibes)** | ISWC Research Team | Custom Haptic Wristband | Continuous vibration feedback corresponding to ambient sound volume | ❌ Loudness sensing only; no sound classification or direction detection |

---

## 🌟 2. Why THIS Project is Unique & Novel

None of the open-source projects currently on GitHub combine all 5 of these core features onto a **standalone ~₹3,300 ($40) microcontroller platform**:

| System Feature | Existing GitHub Projects (e.g. SoundWatch) | YOUR ESP32-S3 Deaf Smart Watch |
| :--- | :---: | :---: |
| **Hardware Platform** | Expensive Commercial WearOS Watch ($200+) | **Low-Cost ESP32-S3 All-in-One Board (~₹1,884 / $22)** |
| **Edge-AI Sound Inference** | Heavy YAMNet Model on Android | **TinyML 1D-CNN Model running on ESP32 Core 0 (18ms)** |
| **Sound Direction Detection** | ❌ No directional alerts | **✅ Dual-Mic Energy Ratio & TDOA Direction Math (Left/Right)** |
| **Two-Way Speech System** | ❌ Notification alerts only | **✅ Deaf user reads voice on watch & plays typed text out loud!** |
| **Emergency SOS Location SMS** | ❌ None | **✅ 3-second SOS button sends live Google Maps pin SMS** |
| **Total System Cost** | ₹20,000+ ($250+) | **~₹3,300 ($40 Total BOM)** |

---

## 🚀 3. Novel Contributions of This Architecture

1. **Ultra-Low-Cost Accessibility:** Brings high-end accessibility features (previously requiring a $250 smartwatch + $500 smartphone) down to an affordable **₹3,300 DIY kit**.
2. **Dual-Core FreeRTOS Partitioning:** Core 0 handles real-time 48kHz audio sampling & 18ms TinyML inference; Core 1 handles GC9A01 LCD SPI rendering, NimBLE Bluetooth, and DRV2605L haptics.
3. **Feedback Prevention Rule:** Auto-pauses Edge Impulse sound classification during TTS playback to prevent speaker feedback loops.
4. **100% Offline Operation:** All sound classification, direction math, and speech-to-text / text-to-speech run offline without requiring paid API keys or active internet.
