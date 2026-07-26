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
Collect data for **7 distinct audio categories**, optimized for Deaf and Hard of Hearing (DHH) life safety, communication, and home awareness:

| Category Label | Priority Tier | Sound Description & Acoustic Profile | Examples to Record | DHH User Utility |
| :--- | :--- | :--- | :--- | :--- |
| `fire_alarm` | 🚨 Emergency | High-pitched continuous beeps ($2.5-3.5\text{ kHz}$) or ISO 8201 T3/T4 patterns | Smoke detector alarms, building evacuation horns, fire alarms | **Top Indoor Life Safety:** Crucial while sleeping or working indoors. |
| `sirens` | 🚨 Emergency | Frequency-sweeping wails/yelps ($500-1500\text{ Hz}$) | Ambulance, police car, fire engine sirens, civil defense horns | **Top Outdoor Safety:** Warns of approaching emergency vehicles while walking. |
| `car_horn` | ⚡ Critical | High-energy harmonic acoustic bursts ($1-3\text{ kHz}$) | Car horn honks, vehicle reverse beepers, traffic alerts | **Roadway Hazard:** Prevents vehicular collisions in parking lots and streets. |
| `phone_ringtone` | 📞 Communication | Rhythmic melodic or digital repeating ring tones | Mobile phone ringtones, landline rings, VoIP call alerts | **Social/Call Alert:** Alerts user to incoming calls when phone is in bag/room. |
| `doorbell_knock` | 🔔 Convenience | Transient impact knock or 2-tone electronic chime | Front door knocking, ding-dong chimes, intercom buzzers | **Domestic Access:** Alerts user to visitors, delivery agents, family. |
| `baby_crying` | 🍼 Caregiver | Periodic infant vocalization pitch contours ($400-600\text{ Hz}$) | Infant crying, baby distress, toddler screams | **Caregiver Alert:** Essential for deaf parents & infant caregivers. |
| `noise` | 🎧 Baseline | Broad spectrum background ambient sound (Negative class) | AC fan hum, street traffic hum, room silence, typing, room chatter | **False Positive Control:** Crucial baseline class for model accuracy. |

### 2. Sound Selection Rationale & Exclusion Criteria

To deploy a high-accuracy model on the ESP32-S3 microcontroller ($\sim 24\text{ KB}$ RAM limit), sound classes are strictly prioritized based on accessibility need and acoustic separability:

* **Why Include Mobile Ringtones (`phone_ringtone`)?**  
  Deaf users rely on visual/tactile alerts for calls (e.g., Video Relay Service, emergency family calls). When the phone is in a bag, pocket, or another room, ringtone sound recognition on the watch ensures key calls are not missed.
* **Why Exclude Dog Barking & Appliance Timers?**  
  * *Microwave/Appliance Beeps:* Single $3\text{kHz}$ sine wave beeps frequently overlap with smoke detector alarms. Excluding appliance timers prevents false fire alarm panics.
  * *Dog Barking:* Barking varies drastically in frequency ($200\text{Hz} - 2\text{kHz}$) and causes high false-positive rates on micro 1D-CNN models.
  * *Glass Breaking & Thunder:* Thunder is felt physically as low-frequency vibration. Glass breaking is a sub-second transient ($<200\text{ms}$) that is unreliable for a 1-second sliding audio window.

### 3. Audio Formatting & Quantity Rules
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
* **Goal:** You should see distinct colored clusters for `fire_alarm`, `sirens`, `car_horn`, `phone_ringtone`, `doorbell_knock`, `baby_crying`, and `noise`. If `sirens` and `car_horn` or `fire_alarm` overlap heavily, add more varied distance training samples.

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
[Dense Output Layer (7 Classes - Softmax)]
```

3. **Hyperparameters:**
   * **Number of Epochs:** `50`
   * **Learning Rate:** `0.005`
   * **Validation Set Size:** `20%`
4. Click **Start Training**.

### Validation Target Metrics:
* **Accuracy:** Target $\ge 90\%$
* **Confusion Matrix:** Verify that `fire_alarm`, `sirens`, and `phone_ringtone` have zero false classifications into `noise`.

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
