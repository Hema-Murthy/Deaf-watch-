# 🎙️ Complete Step-by-Step Edge Impulse Guide for Tactile Smartwatch

This guide provides a neat, visual, step-by-step walkthrough for building, training, and deploying your custom **1D-CNN Environmental Sound Classifier** in Edge Impulse Studio for the ESP32-S3 Deaf Smartwatch project.

---

## 🎯 Phase 1: Project Setup

1. Open [Edge Impulse Studio](https://studio.edgeimpulse.com/) and log in to your account.
2. Select or create your project: **`JEEVAN0714 / smart watch`**.
3. Set your target device hardware to **ESP32-S3** (or Cortex-M4 / Espressif).

---

## 🎙️ Phase 2: Data Acquisition (Audio Dataset)

Collect audio samples for **7 sound classes** (16,000 Hz, 16-bit Mono WAV format):

| Sound Class | Sound Category | Target Duration | DHH Accessibility Utility |
| :--- | :--- | :--- | :--- |
| `fire_alarm` | 🚨 Emergency | 5 - 10 mins | Smoke detector & evacuation alerts while sleeping/indoors |
| `sirens` | 🚨 Emergency | 5 - 10 mins | Ambulance, police, fire engine sirens outdoors |
| `car_horn` | ⚡ Critical | 5 - 10 mins | Vehicular hazard alerts on streets |
| `phone_ringtone` | 📞 Call Alert | 5 - 10 mins | Incoming video/voice calls when phone is in bag |
| `doorbell_knock` | 🔔 Convenience | 5 - 10 mins | Front door visitors, delivery agents, family |
| `baby_crying` | 🍼 Caregiver | 5 - 10 mins | Alert for deaf parents & infant caregivers |
| `noise` | 🎧 Baseline | 5 - 10 mins | AC fan, street traffic, room silence, background hum |

---

## 🌐 🔊 Free Audio Dataset Sources for Data Acquisition

Here are the top 7 free, high-quality audio repositories to download sound clips for your training dataset:

### 1. [Freesound.org](https://freesound.org/) ⭐ *(#1 Recommended Source)*
* **Best For:** All 7 classes (`fire_alarm`, `sirens`, `car_horn`, `doorbell_knock`, `baby_crying`, `phone_ringtone`).
* **How to Search:** Search keywords like `"smoke alarm ISO 8201"`, `"police siren wail"`, `"ding dong doorbell"`, `"baby cry WAV"`, `"car horn honk"`.
* **Format:** Download direct 16-bit PCM WAV or MP3 files.

### 2. [ESC-50 Dataset (Environmental Sound Classification)](https://github.com/karolpiczak/ESC-50)
* **Best For:** Pre-packaged 2,000 environmental clips (5 seconds each, 44.1kHz mono WAV).
* **Contains Classes:** Doorbells, sirens, crying baby, car horns, washing machine hum, ambient room noise.

### 3. [UrbanSound8K Dataset](https://urbansounddataset.weebly.com/urbansound8k.html)
* **Best For:** `car_horn`, `sirens`, `street_noise` baseline.
* **Contains:** 8,732 labeled urban audio clips (10 categories of real outdoor street sounds).

### 4. [Google AudioSet Repository](https://research.google.com/audioset/ontology/index.html)
* **Best For:** Finding labeled YouTube sound clips for rare emergency alarms, siren sweep patterns, and infant distress cries.

### 5. [BBC Sound Effects Library](https://sound-effects.bbcrewind.co.uk/)
* **Best For:** High-fidelity indoor & outdoor ambient noise (`noise` baseline, room hum, rain, street chatter).
* **Catalog:** Over 33,000 royalty-free audio tracks for educational & research projects.

### 6. [Zapsplat Free Sound Effects](https://www.zapsplat.com/)
* **Best For:** Clean studio-recorded ringtones, door knocking transients, household chime sounds.

### 7. Direct Smartphone Recording via Edge Impulse Web Client
* **Best For:** Custom real-world testing data.
* **How to use:** In Edge Impulse Studio $\rightarrow$ **Data acquisition** $\rightarrow$ **Connect a device** $\rightarrow$ **Show QR code**. Scan with your mobile phone to turn your phone into a live 16kHz audio recorder!

---

## ⚙️ Phase 3: Impulse Design (`Create Impulse`)

In the left sidebar menu under **Impulse design**, click **Create Impulse**:

```text
┌───────────────────┐      ┌───────────────────┐      ┌───────────────────┐
│   INPUT BLOCK     │ ───► │ PROCESSING BLOCK  │ ───► │  LEARNING BLOCK   │
│ Time Series Audio │      │  MFE Spectrogram  │      │ Neural Network    │
│ (16kHz, 1000ms)   │      │ (Mel Filterbank)  │      │ (1D-CNN)          │
└───────────────────┘      └───────────────────┘      └───────────────────┘
```

* **Time Series Input Block:**
  * **Window size:** `1000 ms` (1.0 Second)
  * **Window increase:** `250 ms` (Sliding window overlap)
  * **Frequency:** `16000 Hz`
* **Processing Block:** Select **MFE (Mel-Filterbank Energy)**.
* **Learning Block:** Select **Classification (Keras)**.
* Click **Save Impulse**.

---

## 📊 Phase 4: MFE Feature Extraction (`MFE`)

In the left sidebar menu under **Impulse design**, click **MFE**:

1. **Parameters Configuration:**
   * **Frame length:** `0.02s` (20 ms)
   * **Frame stride:** `0.01s` (10 ms)
   * **Filter bands:** `40`
   * **FFT length:** `256`
   * **Low frequency:** `300 Hz`
   * **High frequency:** `0 Hz` (Auto Nyquist limit)
2. Click **Save parameters**.
3. Click **Generate features** (Purple button) and wait for the job to complete.
4. **Inspect Feature Explorer:** Verify that your sound classes form distinct cluster groupings.

---

## 🧠 Phase 5: 1D-CNN Neural Network Training (`Classifier`)

In the left sidebar menu under **Impulse design**, click **Classifier** (located directly below MFE):

```text
  Impulse design
  │
  ├── 🟢 Create Impulse
  ├── 🟢 MFE
  └── ⚪ Classifier        👈 CLICK HERE!
```

### 1. Training Settings:
* **Number of Epochs:** `50`
* **Learning Rate:** `0.005`
* **Validation Set Size:** `20%`

### 2. Neural Network Architecture (1D-CNN):
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

3. Click **Start training** (Purple button).
4. **Verify Target Results:** Accuracy $\ge 90\%$ with minimal false positives between `fire_alarm` and `noise`.

---

## ⚡ Phase 6: Model Quantization (Int8)

1. Scroll down to **Model Version** on the Classifier page after training finishes.
2. Select **Int8 (8-bit Quantized)**.
3. **On-Device Target Performance for ESP32-S3:**
   * **RAM Footprint:** $\sim 24 \text{ KB}$
   * **Flash Footprint:** $\sim 85 \text{ KB}$
   * **Inference Latency:** $\sim 18 \text{ ms}$

---

## 🧪 Phase 7: Model Testing & Verification

1. In the left sidebar, click **Model Testing**.
2. Click **Classify All** to test against unseen evaluation data.
3. Verify test score accuracy is $\ge 88\%$.

---

## 📦 Phase 8: Exporting C++ Arduino Library

1. In the left sidebar, click **Deployment**.
2. Search and select **Arduino Library**.
3. Under Optimization, choose **Quantized (Int8)**.
4. Click **Build** at the bottom to download your `.zip` library (e.g. `ei-jeevan0714-project-1-arduino-1.0.1-impulse-#2.zip`).

---

## 🚀 Phase 9: Arduino IDE / PlatformIO Deployment

1. In Arduino IDE: Go to **Sketch** $\rightarrow$ **Include Library** $\rightarrow$ **Add .ZIP Library...** and select your downloaded `.zip` file.
2. Select Board: **ESP32S3 Dev Module** (Partition: `Huge APP (3MB No OTA)`).
3. Upload the C++ firmware (`main.cpp`) to run TinyML audio inference on **Core 0** and Haptic/BLE/LCD tasks on **Core 1**!

