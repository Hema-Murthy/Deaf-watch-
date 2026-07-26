# Module 02: System Architecture, Dataflow & Logic

## 1. Executive Architecture Overview

The system utilizes an **Edge-AI + BLE Mobile Companion** architecture. Offline real-time safety tasks (sound classification and sound direction detection) execute locally on the ESP32-S3 microcontroller, while heavy natural speech processing and emergency dispatch are handled by the smartphone.

```mermaid
flowchart TD
    subgraph Environment & Acoustic Sensors
        A[Ambient Acoustic Waves] -->|External Person Speaking| B[Mic Left: GPIO 14/15/32]
        A -->|Environmental Sounds| C[Mic Right: GPIO 14/15/33]
    end

    subgraph ESP32-S3 Smartwatch Hardware
        B & C -->|Stereo 48kHz I2S0 DMA Stream| D[FreeRTOS Core 0: DSP & ML Task]
        
        D -->|Energy Ratio & Delay| E{Sound Direction Logic ILD/TDOA}
        D -->|Downsampled 16kHz| F{Edge Impulse 1D-CNN Model}
        
        E -->|Direction Angle: Left/Right| G[FreeRTOS Core 1: Display & UI Task]
        F -->|Confidence > 85%| H[DRV2605L Haptic Driver + LRA Motor]
        F -->|Sound Event Icon| G
        
        G -->|SPI Protocol| I[GC9A01 1.28" Round LCD Display]
        
        J[SOS Push Button] -->|GPIO 4 Interrupt 3s Hold| K[NimBLE Server Task]

        L[MAX98357A I2S Amp + Micro Speaker] <--|I2S1 Digital Audio Stream| K
    end

    subgraph Mobile Companion App (React Native)
        K <-->|BLE 5.0 MTU=247 Audio Packets| M[React Native BLE Manager]
        
        M -->|Voice Input from Watch Mic| N[Free On-Device STT @react-native-voice]
        N -->|Transcribed Text string over BLE| K
        K -->|Preset Phrase Selected| O[Free On-Device TTS react-native-tts]
        O -->|ADPCM Compressed Voice Stream| M
        
        M -->|SOS Signal Received| P[Geolocation Service - Lat/Lng]
        P --> Q[Native SIM SMS Dispatch to Emergency Contacts]
    end
```

---

## 2. Audio & Speech Flow (Smartwatch Centric)

### A. Listening Flow (Speech Received from External Person)
1. Someone speaks to the deaf user $\rightarrow$ **Smartwatch Microphones (INMP441)** capture their voice.
2. ESP32 sends the voice snippet over BLE to the Mobile App.
3. Mobile App runs On-Device Speech-to-Text (STT).
4. Transcribed text string is pushed back over BLE to the watch.
5. **Smartwatch Round LCD Screen** displays the text in real-time so the user reads it!

### B. Speaking Flow (Read Out Loud to Hearing Person)
1. Deaf/Mute user selects a quick phrase (e.g. *"Where is the train station?"*) on the watch interface.
2. Mobile App generates spoken audio using On-Device Text-to-Speech (TTS) and compresses it with ADPCM.
3. ADPCM audio stream is transmitted over BLE back to the ESP32.
4. **Smartwatch MAX98357A Amp & Micro Speaker** plays the voice **OUT LOUD** directly from the smartwatch!

---

## 3. Sound Direction Detection Mathematics (TDOA & ILD)

### The Physics Problem
On a smartwatch dial diameter ($d \approx 40\text{mm} = 0.04\text{m}$), the maximum time difference of arrival ($\Delta t_{max}$) for sound ($c = 343\text{ m/s}$) is:

$$\Delta t_{max} = \frac{d}{c} = \frac{0.04\text{ m}}{343\text{ m/s}} \approx 116.6 \, \mu\text{s}$$

At a 16kHz sampling rate ($62.5\,\mu\text{s}$ per sample), the time delay corresponds to less than 2 sample ticks—making pure time correlation noisy.

### The ESP32-S3 Solution (Stereo 48kHz + Interaural Level Difference)
By sampling stereo audio at **$48\text{kHz}$** ($20.8\,\mu\text{s}$ per sample tick), the ESP32-S3 calculates both **Phase Delay** and **Interaural Level Difference (ILD)** between Mic Left and Mic Right:

$$\text{Energy Ratio} = \frac{\sum_{i=1}^{N} S_{\text{left}}[i]^2}{\sum_{i=1}^{N} S_{\text{right}}[i]^2 + \epsilon}$$

* **If Energy Ratio $> 1.35 \implies$** Sound source is on the **LEFT** side ($\leftarrow$).
* **If Energy Ratio $< 0.74 \implies$** Sound source is on the **RIGHT** side ($\rightarrow$).
* **If $0.74 \le \text{Energy Ratio} \le 1.35 \implies$** Sound source is **AHEAD / CENTER** ($\uparrow$).

---

## 4. FreeRTOS Dual-Core Task Distribution

```text
               +-------------------------------------------------+
               |              ESP32-S3 DUAL-CORE MCU             |
               +-------------------------------------------------+
                                /               \
                               /                 \
            CORE 0 (DSP & AI)               CORE 1 (UI & BLE)
      +---------------------------+   +---------------------------+
      | - 48kHz I2S0 DMA Sampling |   | - GC9A01 LCD SPI Renderer |
      | - ILD/TDOA Direction Math |   | - DRV2605L I2C Haptics    |
      | - Edge Impulse Inference  |   | - NimBLE GATT Server      |
      |                           |   | - I2S1 MAX98357A Speaker  |
      +---------------------------+   +---------------------------+
```

---

## 5. BLE ADPCM Audio Streaming & Concurrency Rules

### A. ADPCM Compression Stream
* **Problem:** Raw 16kHz 16-bit audio requires $256\text{ kbps}$, exceeding reliable BLE throughput ($100 - 150\text{ kbps}$).
* **Solution:** The companion app compresses speech using **IMA ADPCM** ($4:1$ ratio), reducing bandwidth to **$64\text{ kbps}$**. The ESP32 decodes ADPCM in real-time and streams it to the MAX98357A speaker via I2S1.

### B. Feedback Prevention Auto-Pause Rule
* **Auto-Pause:** Whenever active speech recording or MAX98357A speaker playback is active, **Core 0 automatically pauses Edge Impulse sound classification**.
* **Resume:** Sound classification resumes 500ms after audio playback stops, preventing speaker feedback from triggering false siren/doorbell alerts.
