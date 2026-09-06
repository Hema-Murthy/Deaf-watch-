#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// 1. INMP441 I2S Microphone Pin Definitions (Standard ESP32-S3)
// ============================================================================
#define I2S_WS_PIN        42    // LRCL / Word Select
#define I2S_SD_PIN        41    // DOUT / Data Out
#define I2S_SCK_PIN       40    // BCLK / Bit Clock
#define I2S_PORT          I2S_NUM_0

// Audio Sampling Parameters
#define SAMPLE_RATE       16000 // 16 kHz sample rate (TinyML standard)
#define SAMPLE_BITS       16    // 16-bit PCM Mono
#define EI_CLASSIFIER_RAW_SAMPLE_COUNT 16000 // 1.0 Second audio window

// ============================================================================
// 2. Tactile / Haptic Vibration Motor Pin
// ============================================================================
#define VIBRATION_MOTOR_PIN  15  // Transistor / MOSFET Gate drive pin for vibration motor

// ============================================================================
// 3. Status LED / OLED Display Pins (Optional)
// ============================================================================
#define OLED_SDA_PIN      8
#define OLED_SCL_PIN      9

// ============================================================================
// 4. Bluetooth Low Energy (BLE) GATT Custom UUIDs
// ============================================================================
#define BLE_DEVICE_NAME             "DeafWatch-SoundAI"
#define SERVICE_UUID                "4fa8691a-1ba6-48fb-a642-155514362500"
#define CHARACTERISTIC_UUID_ALERT   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_STATUS  "825e0766-8802-4029-9e8c-529a65f90383"

// ============================================================================
// 5. Sound Category Class IDs & Thresholds
// ============================================================================
#define CONFIDENCE_THRESHOLD        0.75f  // Minimum 75% score to trigger tactile alert

enum SoundClassID {
    SOUND_FIRE_ALARM = 0,
    SOUND_SIRENS,
    SOUND_CAR_HORN,
    SOUND_ALARM,             // Phone Alarm / Clock Alarm
    SOUND_DOORBELL_KNOCK,
    SOUND_BABY_CRYING,
    SOUND_NOISE
};

#endif // CONFIG_H
