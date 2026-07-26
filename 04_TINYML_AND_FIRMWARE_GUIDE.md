# Module 04: Edge Impulse TinyML Audio Model Training Guide

This guide provides a comprehensive, step-by-step walkthrough for collecting audio data, designing the signal processing pipeline, and training your custom **Environmental Sound Classifier** using **Edge Impulse Studio** for your ESP32-S3 smartwatch.

---

## 🎯 Phase 1: Edge Impulse Project Setup

1. Go to [Edge Impulse Studio](https://edgeimpulse.com/) and log in (or create a free developer account).
2. Click **Create new project** and name it `Tactile-Smartwatch-Audio`.
3. Select **Audio / Sound** as your project type.

---

## 🎙️ Phase 2: Audio Dataset Collection & Data Rules

A neural network is only as good as its training dataset. Follow these data rules carefully:

### 1. Target Sound Classes
Collect data for **6 distinct audio categories**:

| Category Label | Sound Description | Examples to Record |
| :--- | :--- | :--- |
| `sirens` | Emergency vehicle horns/sirens | Police car sirens, ambulance, fire truck horns |
| `doorbell` | Door knocking & electronic chimes | Front door knocking, ding-dong chimes, intercom buzzers |
| `fire_alarm` | High-pitched continuous alarms | Smoke detector beeps, building fire alarms |
| `baby_crying` | Infant crying & distress sounds | Baby crying audio samples |
| `dog_barking` | Dog barks & pet alerts | Dog barking, guard dog alerts |
| `noise` | Background ambient sounds | Fan hums, street traffic, room silence, background TV |

### 2. Audio Formatting & Quantity Rules
* **Sample Rate:** `16,000 Hz` (16kHz), 16-bit Mono WAV format.
* **Duration per Class:** Aim for **5 to 10 minutes** of total audio per category.
* **Data Splitting:** Maintain an **80% Training / 20% Testing** split.
* **Environmental Variety:** Record sounds at different distances ($1\text{m}$, $3\text{m}$, $5\text{m}$) and in different room acoustics (bedroom, outdoor street, kitchen).

---

## ⚙️ Phase 3: Designing the Impulse (Signal Pipeline)

In Edge Impulse, navigate to **Create Impulse** in the sidebar:

```text
+-------------------+      +-------------------+      +-------------------+
|   INPUT BLOCK     | ---> | PROCESSING BLOCK  | ---> |  LEARNING BLOCK   |
| Time Series Audio |      |  MFE Spectrogram  |      | Neural Network    |
| (16kHz, 1000ms)   |      | (Mel Filterbank)  |      | (1D-CNN)          |
+-------------------+      +-------------------+      +-------------------+
```

### Configure Block Settings:
1. **Time Series Input Block:**
   * **Sample Frequency:** `16000 Hz`
   * **Window Size:** `1000 ms` (1.0 second)
   * **Window Increase:** `250 ms` (Sliding window overlap for continuous background inference)
2. **Processing Block:** Select **MFE (Mel-Filterbank Energy)**.
   * *Why MFE instead of MFCC?* MFE preserves non-human environmental audio frequencies much better than MFCC (which is tuned strictly for human speech).
3. **Learning Block:** Select **Classification (Keras)**.

---

## 📊 Phase 4: Spectrogram Feature Extraction

1. Click on the **MFE** tab in the sidebar.
2. **Parameters Configuration:**
   * **Frame Length:** `0.02s` ($20\text{ms}$)
   * **Frame Stride:** `0.01s` ($10\text{ms}$)
   * **Filter Bands:** `40`
   * **FFT Length:** `256`
3. Click **Save Parameters** $\rightarrow$ then click **Save & Calculate Features**.

### Feature Explorer Inspection
* Look at the 3D Feature Cluster visualization.
* **Goal:** You should see distinct colored clusters for `sirens`, `doorbell`, `fire_alarm`, and `noise`. If `sirens` and `fire_alarm` overlap heavily, add more distinct training samples.

---

## 🧠 Phase 5: Training the Neural Network (1D-CNN)

1. Click on the **Classifier** tab in the sidebar.
2. **Neural Network Architecture Setup:**

```text
[Input Features: 40 MFE Bands]
           │
           ▼
[1D Convolutional Layer (8 Filters, Kernel Size 3)] ---> [MaxPool 1D]
           │
           ▼
[1D Convolutional Layer (16 Filters, Kernel Size 3)] ---> [MaxPool 1D]
           │
           ▼
[Dropout Layer (0.25 Rate)]  <-- Prevents Overfitting
           │
           ▼
[Dense Output Layer (6 Classes - Softmax)]
```

3. **Hyperparameters:**
   * **Number of Epochs:** `50`
   * **Learning Rate:** `0.005`
   * **Validation Set Size:** `20%`
4. Click **Start Training**.

### Validation Target Metrics:
* **Accuracy:** Target $\ge 90\%$
* **Confusion Matrix:** Verify that `sirens` and `doorbell` have zero false classifications into `noise`.

---

## ⚡ Phase 6: Model Quantization & Optimization (EON Engine)

1. Scroll down to **Model Version** after training.
2. Select **Int8 (8-bit Quantized Model)** instead of Float32.
   * *Quantization Advantage:* Reduces model memory size by $4\times$ with $< 1\%$ loss in accuracy!
3. **On-Device Target Specs for ESP32-S3:**
   * **Peak RAM Usage:** $\sim 24 \text{ KB}$
   * **Flash Footprint:** $\sim 85 \text{ KB}$
   * **Inference Latency:** $\sim 18 \text{ ms}$ on ESP32-S3

---

## 🧪 Phase 7: Model Testing & Verification

1. Go to **Model Testing** in the sidebar.
2. Click **Classify All** to run the trained model on your unseen 20% test dataset.
3. **Live Testing:** Use the Edge Impulse mobile phone web client or laptop microphone to play siren/doorbell audio from your phone and verify live detection confidence score ($> 85\%$).
