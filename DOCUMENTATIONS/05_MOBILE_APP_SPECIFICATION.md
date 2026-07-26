# Module 05: Mobile Companion App Specification

## 1. App Architecture & Package Stack

The companion smartphone app is built with **React Native**, serving as the communication bridge, speech engine, and emergency dispatcher for the smartwatch.

| Functionality | React Native Package | Cost | Network Requirement |
| :--- | :--- | :--- | :--- |
| **BLE Communication** | `react-native-ble-manager` | **$0** | Bluetooth 5.0 Local |
| **Speech-to-Text (STT)** | `@react-native-voice/voice` | **$0** | Works 100% Offline (Native Android/iOS) |
| **Text-to-Speech (TTS)** | `react-native-tts` | **$0** | Works 100% Offline (Native Android/iOS) |
| **GPS Location** | `react-native-geolocation-service` | **$0** | On-Device GPS Hardware |
| **Emergency SOS SMS** | `react-native-sms` / `Linking.openURL` | **$0** | Native SIM Cellular Network |

---

## 2. Emergency SOS SIM SMS Dispatch Logic

When the user holds the physical watch SOS button for 3 seconds:
1. ESP32 sends a high-priority interrupt packet over BLE characteristic `0x2A06`.
2. Mobile app intercepts the BLE notification.
3. App fetches high-accuracy GPS coordinates (`Latitude`, `Longitude`).
4. App generates a native SIM SMS message to Emergency Contacts:

```typescript
import SendSMS from 'react-native-sms';
import Geolocation from 'react-native-geolocation-service';

function handleEmergencySOS(emergencyContacts: string[]) {
    Geolocation.getCurrentPosition(
        (position) => {
            const { latitude, longitude } = position.coords;
            const googleMapsUrl = `https://maps.google.com/?q=${latitude},${longitude}`;
            const messageBody = `🚨 EMERGENCY SOS ALERT! I need help. My current location: ${googleMapsUrl}`;

            SendSMS.send({
                body: messageBody,
                recipients: emergencyContacts,
                allowExecution: true
            }, (completed, cancelled, error) => {
                console.log('SOS SMS Sent Result:', completed);
            });
        },
        (error) => console.error(error),
        { enableHighAccuracy: true, timeout: 15000, maximumAge: 10000 }
    );
}
```

---

## 3. Two-Way Speech Interaction Flow

```text
[User Speaks to Watch Mic] ---> [Raw Audio to BLE] ---> [App On-Device STT] ---> [Text on Watch Screen]
[User Types on Watch/App]  ---> [App On-Device TTS] ---> [ADPCM Audio to BLE] ---> [Watch Speaker Plays Audio]
```

### Why Recipient Contacts Need NO App Installed:
The emergency alert is delivered as a **standard cellular SMS text message**. When the emergency contact opens their text inbox on any smartphone (Android or iPhone), tapping the Google Maps URL instantly opens Google Maps showing your live pin location!
