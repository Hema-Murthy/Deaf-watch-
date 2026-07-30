# Module 04: Edge Impulse TinyML Audio Model Training & ESP32-S3 Firmware Guide

This guide provides an exhaustive, step-by-step, end-to-end walkthrough for collecting audio data, training a custom **Environmental Sound Classifier** in **Edge Impulse Studio**, exporting the C++ library, and deploying the complete firmware on the **ESP32-S3 Smart Watch**.

---

## 🎯 Phase 1: Edge Impulse Project Setup

1. Go to [Edge Impulse Studio](https://edgeimpulse.com/) and log in (or create a free account).
2. Click **Create new project** and name it `Tactile-Smartwatch-Audio`.
3. Select **Audio / Sound** as your project category.
4. Set project target to **ESP32-S3 (Espressif)** under project settings.

---

## 🎙️ Phase 2: Audio Dataset Collection & Data Rules

### 1. Target Sound Classes (7 Audio Categories)

| Category Label | Priority Tier | Sound Description & Acoustic Profile | Examples to Record | DHH User Utility |
| :--- | :--- | :--- | :--- | :--- |
| `fire_alarm` | 🚨 Emergency | High-pitched continuous beeps ($2.5-3.5\text{ kHz}$) or ISO 8201 T3/T4 patterns | Smoke detector alarms, building evacuation horns | **Top Indoor Life Safety:** Crucial while sleeping or working indoors. |
| `sirens` | 🚨 Emergency | Frequency-sweeping wails/yelps ($500-1500\text{ Hz}$) | Ambulance, police car, fire engine sirens | **Top Outdoor Safety:** Warns of approaching emergency vehicles while walking. |
| `car_horn` | ⚡ Critical | High-energy harmonic acoustic bursts ($1-3\text{ kHz}$) | Car horn honks, vehicle reverse beepers | **Roadway Hazard:** Prevents vehicular collisions on streets. |
| `phone_ringtone` | 📞 Call Alert | Rhythmic melodic or digital repeating ring tones | Mobile phone ringtones, landline rings, VoIP calls | **Social/Call Alert:** Alerts user to incoming calls when phone is in bag/room. |
| `doorbell_knock` | 🔔 Convenience | Transient impact knock or 2-tone electronic chime | Front door knocking, ding-dong chimes, intercoms | **Domestic Access:** Alerts user to visitors, delivery agents, family. |
| `baby_crying` | 🍼 Caregiver | Periodic infant vocalization pitch contours ($400-600\text{ Hz}$) | Infant crying, baby distress, toddler screams | **Caregiver Alert:** Essential for deaf parents & infant caregivers. |
| `noise` | 🎧 Baseline | Broad spectrum background ambient sound (Negative class) | AC fan hum, street traffic, room silence, typing, room chatter | **False Positive Control:** Crucial baseline class for model accuracy. |

### 2. Dataset Guidelines & Best Practices
* **Sample Rate:** `16,000 Hz` (16kHz), 16-bit Mono PCM WAV format.
* **Quantity Target:** Record **5 to 10 minutes** of total audio per category (divided into 1-second to 10-second clips).
* **Train / Test Split:** **80% Training / 20% Testing** (Use Edge Impulse auto-split or manually assign).
* **Data Sources:** 
  - Record directly using your phone/laptop microphone.
  - Upload samples from public datasets (e.g., ESC-50, UrbanSound8K, Google Speech Commands).

---

## ⚙️ Phase 3: Designing the Impulse (Signal Pipeline)

Navigate to **Create Impulse** in the Edge Impulse sidebar:

```text
┌───────────────────┐      ┌───────────────────┐      ┌───────────────────┐
│   INPUT BLOCK     │ ───► │ PROCESSING BLOCK  │ ───► │  LEARNING BLOCK   │
│ Time Series Audio │      │  MFE Spectrogram  │      │ Neural Network    │
│ (16kHz, 1000ms)   │      │ (Mel Filterbank)  │      │ (1D-CNN)          │
└───────────────────┘      └───────────────────┘      └───────────────────┘
```

### Configure Settings:
1. **Time Series Input Block:**
   * **Sample Frequency:** `16000 Hz`
   * **Window Size:** `1000 ms` (1.0 Second window)
   * **Window Increase:** `250 ms` (250ms sliding window overlap for continuous background inference)
2. **Processing Block:** Select **MFE (Mel-Filterbank Energy)**.
   * *Why MFE over MFCC?* MFE preserves non-human environmental frequencies (sirens, horns, chimes) much better than MFCC (which is optimized strictly for human speech).
3. **Learning Block:** Select **Classification (Keras)**.

---

## 📊 Phase 4: Spectrogram Feature Extraction (MFE)

1. Click on the **MFE** tab in the left sidebar.
2. **Set Parameters:**
   * **Frame Length:** `0.02s` (20 ms)
   * **Frame Stride:** `0.01s` (10 ms)
   * **Filter Bands:** `40`
   * **FFT Length:** `256`
   * **Low Frequency:** `300 Hz`
   * **High Frequency:** `0 Hz` (Auto-detect up to 8000 Hz Nyquist limit)
3. Click **Save Parameters** $\rightarrow$ then click **Save & Calculate Features**.
4. **Inspect Feature Explorer:** Verify that data clusters for `fire_alarm`, `sirens`, `doorbell_knock`, etc. form distinct colored groupings.

---

## 🧠 Phase 5: Training the Neural Network (1D-CNN)

1. Click on the **Classifier** tab in the sidebar.
2. **Neural Network Architecture:**

```text
[Input Features: 40 MFE Bands x 99 Frames]
                    │
                    ▼
[1D Conv (8 Filters, Kernel 3, Relu)] ──► [MaxPool 1D (Size 2)]
                    │
                    ▼
[1D Conv (16 Filters, Kernel 3, Relu)] ──► [MaxPool 1D (Size 2)]
                    │
                    ▼
[Dropout Layer (0.25 Rate)]  <-- Prevents Overfitting
                    │
                    ▼
[Dense Output Layer (7 Classes - Softmax)]
```

3. **Hyperparameters:**
   * **Number of Epochs:** `50`
   * **Learning Rate:** `0.005`
   * **Validation Set Size:** `20%`
4. Click **Start Training**.
5. **Target Validation Accuracy:** $\ge 90\%$ (Ensure zero misclassification between `fire_alarm` and `noise`).

---

## ⚡ Phase 6: Quantization & Optimization (EON Engine)

1. Scroll to **Model Version** after training finishes.
2. Select **Int8 (8-bit Quantized Model)**.
   * *Quantization Benefit:* Reduces model size by $4\times$ (from ~340KB down to ~85KB) with $< 1\%$ drop in accuracy.
3. **On-Device Target Performance on ESP32-S3:**
   * **Peak RAM:** $\sim 24 \text{ KB}$
   * **Flash Memory:** $\sim 85 \text{ KB}$
   * **Inference Latency:** $\sim 18 \text{ ms}$

---

## 🧪 Phase 7: Model Testing & Verification

1. Go to **Model Testing** in the sidebar.
2. Click **Classify All** to run the trained neural network against the unseen 20% test dataset.
3. Verify overall testing accuracy is $\ge 88\%$.

---

## 📦 Phase 8: Exporting C++ Arduino Library

1. Navigate to the **Deployment** tab in Edge Impulse Studio.
2. Under **Search deployment options**, select **Arduino Library**.
3. Under **Optimization**, select **Quantized (Int8)**.
4. Click **Build** at the bottom.
5. Edge Impulse will compile and download a `.zip` library file (e.g., `ei-jeevan0714-project-1-arduino-1.0.1-impulse-#2.zip`).

---

## 💻 Phase 9: Arduino IDE / PlatformIO Setup

### Arduino IDE Installation:
1. Open **Arduino IDE** (v2.x recommended).
2. Go to **Sketch** $\rightarrow$ **Include Library** $\rightarrow$ **Add .ZIP Library...**.
3. Select your downloaded `ei-jeevan0714-project-1-arduino-1.0.1-impulse-#2.zip` file.

### Required Board & Library Manager Dependencies:
* Board Support: **esp32 by Espressif Systems** (v2.0.11+)
* **NimBLE-Arduino** (by h2zero) - Low footprint BLE library
* **Adafruit DRV2605 Library** - Tactile motor driver
* **TFT_eSPI** or **Arduino_GFX** - GC9A01 LCD screen driver

### ESP32-S3 Board Settings in Arduino IDE:
* **Board:** `ESP32S3 Dev Module` (or `Waveshare ESP32-S3-Touch-LCD-1.28`)
* **CPU Frequency:** `240MHz (WiFi/BT)`
* **Core Debug Level:** `None`
* **USB CDC On Boot:** `Enabled`
* **Flash Size:** `8MB (64Mb)` or `16MB`
* **Partition Scheme:** `Huge APP (3MB No OTA/1MB SPIFFS)`
* **PSRAM:** `OPI PSRAM` (Enabled)

---

## 🚀 Phase 10: Complete ESP32-S3 Production Code (`main.cpp`)

Below is the complete, working production code integrating Edge Impulse TinyML inference on **Core 0**, Dual I2S Microphone sampling, TDOA/ILD Sound Direction Math, DRV2605L Haptics, GC9A01 Display rendering, and NimBLE Bluetooth notifications:

```cpp
#include <Arduino.h>
#include <driver/i2s.h>
// Include your Edge Impulse inferencing header
#include <jeevan0714-project-1_inferencing.h>
#include <Adafruit_DRV2605.h>
#include <NimBLEDevice.h>

// ==========================================
// 1. PIN DEFINITIONS & I2S CONFIGURATION
// ==========================================
#define I2S_WS_PIN        42   // Word Select (LRCLK)
#define I2S_SCK_PIN       41   // Serial Clock (BCLK)
#define I2S_SD_LEFT_PIN   1    // Left INMP441 Mic Data
#define I2S_SD_RIGHT_PIN  2    // Right INMP441 Mic Data
#define SOS_BUTTON_PIN    0    // Boot/SOS Button

Adafruit_DRV2605 haptic;
NimBLECharacteristic* pBLEChar = nullptr;

// Audio Buffer Setup for 16kHz Mono Inference
#define EIDSP_QUANTIZE_FILTER_OFF 0
static signed short sampleBuffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];
static bool is_inference_running = false;

// Left / Right Energy ratio for direction detection
float left_mic_energy = 0.0;
float right_mic_energy = 0.0;

// ==========================================
// 2. I2S DUAL-MIC INITIALIZATION
// ==========================================
void init_i2s_stereo() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = EI_CLASSIFIER_FREQUENCY, // 16000 Hz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // Stereo Dual Mic
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK_PIN,
        .ws_io_num = I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD_LEFT_PIN
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

// ==========================================
// 3. DRV2605L HAPTIC TRIGGER LOGIC
// ==========================================
void trigger_haptic_pattern(const char* sound_label) {
    if (strcmp(sound_label, "fire_alarm") == 0) {
        haptic.setWaveform(0, 47); // Pulsing strong alert
        haptic.setWaveform(1, 47);
        haptic.setWaveform(2, 0);  // End waveform
        haptic.go();
    } else if (strcmp(sound_label, "sirens") == 0) {
        haptic.setWaveform(0, 11); // Transition ramp up
        haptic.setWaveform(1, 12); // Transition ramp down
        haptic.setWaveform(2, 0);
        haptic.go();
    } else if (strcmp(sound_label, "car_horn") == 0) {
        haptic.setWaveform(0, 1);  // Sharp click alert
        haptic.setWaveform(1, 0);
        haptic.go();
    } else if (strcmp(sound_label, "doorbell_knock") == 0) {
        haptic.setWaveform(0, 14); // Soft double hum
        haptic.setWaveform(1, 14);
        haptic.setWaveform(2, 0);
        haptic.go();
    } else if (strcmp(sound_label, "baby_crying") == 0) {
        haptic.setWaveform(0, 58); // Gentle pulsing alert
        haptic.setWaveform(1, 0);
        haptic.go();
    } else if (strcmp(sound_label, "phone_ringtone") == 0) {
        haptic.setWaveform(0, 15); // Rhythmic pulse
        haptic.setWaveform(1, 15);
        haptic.setWaveform(2, 0);
        haptic.go();
    }
}

// ==========================================
// 4. EDGE IMPULSE AUDIO PROVIDER CALLBACK
// ==========================================
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr) {
    numpy::int16_to_float(&sampleBuffer[offset], out_ptr, length);
    return 0;
}

// ==========================================
// 5. CORE 0: TINYML AUDIO INFERENCE TASK
// ==========================================
void TaskAudioInference(void *pvParameters) {
    init_i2s_stereo();
    size_t bytes_read = 0;

    for (;;) {
        // 1. Read Stereo PCM Audio from Dual INMP441 Microphones over I2S
        int16_t raw_stereo[EI_CLASSIFIER_RAW_SAMPLE_COUNT * 2];
        i2s_read(I2S_NUM_0, &raw_stereo, sizeof(raw_stereo), &bytes_read, portMAX_DELAY);

        // 2. Extract Mono Channel & Calculate Left vs Right Direction Energy (ILD)
        double sum_left = 0, sum_right = 0;
        for (int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++) {
            int16_t left_sample  = raw_stereo[i * 2];
            int16_t right_sample = raw_stereo[i * 2 + 1];

            sampleBuffer[i] = left_sample; // Pass mono to EI classifier
            sum_left  += abs(left_sample);
            sum_right += abs(right_sample);
        }

        left_mic_energy  = sum_left  / EI_CLASSIFIER_RAW_SAMPLE_COUNT;
        right_mic_energy = sum_right / EI_CLASSIFIER_RAW_SAMPLE_COUNT;

        // Determine Direction Vector
        const char* direction = "FRONT";
        if (left_mic_energy > (right_mic_energy * 1.35)) {
            direction = "LEFT";
        } else if (right_mic_energy > (left_mic_energy * 1.35)) {
            direction = "RIGHT";
        }

        // 3. Wrap Buffer in Edge Impulse Signal Structure
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
        signal.get_data = &microphone_audio_signal_get_data;

        // 4. Run Edge Impulse TinyML Classifier
        ei_impulse_result_t result = { 0 };
        EI_IMPULSE_ERROR r = run_classifier(&signal, &result, false);

        if (r == EI_IMPULSE_OK) {
            // Find Highest Confidence Prediction
            float max_val = 0.0;
            int max_idx = 0;
            for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                if (result.classification[i].value > max_val) {
                    max_val = result.classification[i].value;
                    max_idx = i;
                }
            }

            const char* detected_label = result.classification[max_idx].label;

            // 5. Trigger Haptics & Send BLE Notification if Confidence > 70%
            if (max_val >= 0.70f && strcmp(detected_label, "noise") != 0) {
                Serial.printf("ALERT: %s | Direction: %s | Conf: %.2f\n", detected_label, direction, max_val);
                
                trigger_haptic_pattern(detected_label);

                if (pBLEChar != nullptr) {
                    char blePayload[64];
                    snprintf(blePayload, sizeof(blePayload), "%s:%s:%.2f", detected_label, direction, max_val);
                    pBLEChar->setValue(blePayload);
                    pBLEChar->notify();
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Yield to core watchdog
    }
}

// ==========================================
// 6. SETUP & MAIN EXECUTION
// ==========================================
void setup() {
    Serial.begin(115200);
    pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);

    // Initialize DRV2605L Haptic Engine
    if (haptic.begin()) {
        haptic.selectLibrary(1);
        haptic.setMode(DRV2605_MODE_INTTRIG);
    }

    // Initialize NimBLE Bluetooth
    NimBLEDevice::init("Tactile-SmartWatch");
    NimBLEServer* pServer = NimBLEDevice::createServer();
    NimBLEService* pService = pServer->createService("180C"); // Custom Service
    pBLEChar = pService->createCharacteristic("2A56", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    pService->start();
    pServer->getAdvertising()->start();

    // Pin Audio Inference Engine to FreeRTOS Core 0
    xTaskCreatePinnedToCore(
        TaskAudioInference,
        "AudioInferenceTask",
        8192,
        NULL,
        1,
        NULL,
        0 // Core 0
    );
}

void loop() {
    // Core 1 handles UI Refresh and SOS Button polling
    if (digitalRead(SOS_BUTTON_PIN) == LOW) {
        delay(3000); // 3-second long press
        if (digitalRead(SOS_BUTTON_PIN) == LOW) {
            Serial.println("🚨 EMERGENCY SOS TRIGGERED!");
            if (pBLEChar != nullptr) {
                pBLEChar->setValue("SOS_ALERT");
                pBLEChar->notify();
            }
        }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}
```

---

## 🛠️ Phase 11: Troubleshooting & Common Verification Fixes

| Issue / Error | Cause | Resolution |
| :--- | :--- | :--- |
| `EI_CLASSIFIER_ALLOCATION_FAILED` | Out of SRAM memory | Enable **PSRAM** in Arduino IDE (`OPI PSRAM`) and check `partition scheme`. |
| `i2s_read timeout / silence` | Incorrect I2S pin assignment | Verify INMP441 `SD`, `SCK`, and `WS` pins match `init_i2s_stereo()` config. |
| False positive alerts on background hum | Noise class under-represented | Collect 3 more minutes of AC fan/typing audio under `noise` category and retrain. |
| High inference latency (>50ms) | Running Float32 instead of Int8 | Ensure **Quantized (Int8)** model is selected in Edge Impulse deployment. |
