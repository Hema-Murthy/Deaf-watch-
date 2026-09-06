# 📱 Custom Companion Mobile App BLE Protocol Specification

This document defines the **Bluetooth Low Energy (BLE) GATT API Protocol** between the **ESP32-S3 Deaf Smart Watch** and your **Custom Companion Mobile Application** (React Native, Flutter, iOS Swift, or Android Kotlin/Java).

---

## 📡 1. BLE Device Identification

* **Advertised Device Name:** `DeafWatch-SoundAI`
* **Primary Service UUID:** `4fa8691a-1ba6-48fb-a642-155514362500`

---

## 🔑 2. GATT Characteristics Table

| Characteristic Name | UUID | Permissions | Description / Usage |
| :--- | :--- | :--- | :--- |
| **`SOUND_ALERT`** | `beb5483e-36e1-4688-b7f5-ea07361b26a8` | `READ`, `NOTIFY` | Real-time notification emitted whenever a sound category is detected above $\ge 75\%$ confidence. |
| **`SYSTEM_STATUS`** | `825e0766-8802-4029-9e8c-529a65f90383` | `READ` | Battery percentage, mic health status, and active model version. |

---

## 📦 3. `SOUND_ALERT` Notification Payload (JSON)

When the ESP32-S3 detects a target sound category, it emits a UTF-8 JSON notification on the `SOUND_ALERT` characteristic:

### Example Payload:
```json
{
  "event": "SOUND_DETECTED",
  "label": "alarm",
  "confidence": 0.92,
  "timestamp": 142100
}
```

### Sound Labels Enum & App Actions

| Label | Category | Priority Tier | Recommended Mobile App Action |
| :--- | :--- | :--- | :--- |
| `fire_alarm` | 🚨 Emergency | Critical High | Full-screen red strobe alert + Full phone vibration + Loud audio chime + Push SMS to emergency contact |
| `sirens` | 🚨 Emergency | High | Red warning pop-up + Phone vibration alert |
| `car_horn` | ⚡ Hazard | High | Orange banner notification + Double phone vibrate |
| `alarm` | ⏰ Clock Alarm | High | Yellow wake-up banner + Persistent phone vibrate |
| `doorbell_knock` | 🔔 Visitor | Medium | Green banner notification + "Visitor at Door" pop-up |
| `baby_crying` | 🍼 Caregiver | High | Yellow banner notification + "Baby Crying Alert" popup |
| `noise` | 🎧 Ambient | Baseline | Ignored by mobile app |

---

## 🛠️ 4. Integration Example Code Snippets

### React Native (`react-native-ble-plx`)
```javascript
import { BleManager } from 'react-native-ble-plx';

const manager = new BleManager();
const SERVICE_UUID = "4fa8691a-1ba6-48fb-a642-155514362500";
const ALERT_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

function subscribeToWatchAlerts(device) {
    device.monitorCharacteristicForService(SERVICE_UUID, ALERT_CHAR_UUID, (error, characteristic) => {
        if (error) {
            console.error("BLE Error:", error);
            return;
        }
        const rawJson = Buffer.from(characteristic.value, 'base64').toString('utf-8');
        const alertData = JSON.parse(rawJson);
        console.log("Detected Sound Alert:", alertData.label, "Confidence:", alertData.confidence);
        
        // Trigger In-App Screen Alert & Push Notification
        triggerAppSoundAlert(alertData);
    });
}
```

### Flutter (`flutter_blue_plus`)
```dart
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'dart:convert';

void listenToWatchAlerts(BluetoothCharacteristic alertChar) async {
    await alertChar.setNotifyValue(true);
    alertChar.onValueReceived.listen((value) {
        String jsonString = utf8.decode(value);
        Map<String, dynamic> data = jsonDecode(jsonString);
        print("Sound Event Received: ${data['label']} (${data['confidence']})");
    });
}
```
