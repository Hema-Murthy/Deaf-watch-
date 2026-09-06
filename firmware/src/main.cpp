/*
 * Tactile Smart Watch - 7-Class Audio Scene Recognition Firmware
 * 
 * Hardware: Standard ESP32-S3 DevKit + INMP441 I2S Mic + Vibration Motor
 * AI Model: 1D-CNN (MFE Spectrogram) Trained via Edge Impulse Studio
 * Output: Tactile Vibration + Custom Mobile App BLE Push Notification
 */

#include <Arduino.h>
#include "config.h"
#include "i2s_microphone.h"
#include "haptic_patterns.h"
#include "ble_companion.h"

// Note: Replace with exported Edge Impulse library header after building in Studio
// #include <Tactile-Smartwatch-Audio_inferencing.h>

// Global Instances
I2SMicrophone mic;
HapticFeedback haptic;
BLECompanion ble;

// Audio Buffer for TinyML Inference
int16_t raw_audio_buffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];

// Task Handles
TaskHandle_t AudioTaskHandle = NULL;

// Mock / Wrapper Classifier Function (To be linked with exported Edge Impulse SDK)
void run_audio_inference() {
    // 1. Read 1 second of 16kHz audio from INMP441
    size_t samples_read = mic.read_samples(raw_audio_buffer, EI_CLASSIFIER_RAW_SAMPLE_COUNT);
    if (samples_read < EI_CLASSIFIER_RAW_SAMPLE_COUNT) {
        return;
    }

    /* 
     * Edge Impulse SDK Integration Snippet:
     * 
     * signal_t signal;
     * signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
     * signal.get_data = &raw_feature_get_data;
     * 
     * ei_impulse_result_t result = { 0 };
     * EI_IMPULSE_ERROR r = run_classifier(&signal, &result, false);
     * 
     * for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
     *     if (result.classification[ix].value > CONFIDENCE_THRESHOLD) {
     *         const char* detected_label = result.classification[ix].label;
     *         float score = result.classification[ix].value;
     *         
     *         if (strcmp(detected_label, "noise") != 0) {
     *             haptic.trigger_alert(detected_label);
     *             ble.sendSoundAlert(detected_label, score);
     *         }
     *     }
     * }
     */
}

// FreeRTOS Core 0 Task: Continuous Audio Sampling & Inference
void AudioInferenceTask(void * pvParameters) {
    Serial.println("[FreeRTOS Core 0] Audio Sampling & TinyML Inference Task Running.");
    for (;;) {
        run_audio_inference();
        vTaskDelay(pdMS_TO_TICKS(10)); // Yield to prevent watchdog reset
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Serial.println("==================================================");
    Serial.println(" ESP32-S3 Deaf Smart Watch - TinyML Firmware");
    Serial.println(" Target: 7 Sound Classes + Haptics + Custom BLE App");
    Serial.println("==================================================");

    // Initialize Peripherals
    haptic.init();
    ble.init();

    if (!mic.init()) {
        Serial.println("CRITICAL ERROR: Failed to initialize INMP441 Microphone!");
    }

    // Launch Audio Inference Task pinned to Core 0
    xTaskCreatePinnedToCore(
        AudioInferenceTask,   /* Task function */
        "AudioTask",          /* Name of task */
        8192,                 /* Stack size in words */
        NULL,                 /* Parameter of the task */
        1,                    /* Priority of the task */
        &AudioTaskHandle,     /* Task handle */
        0                     /* Pin task to Core 0 */
    );

    Serial.println("Setup Complete. Core 1 handling System & BLE.");
}

void loop() {
    // Core 1 Main Loop handles housekeeping / watchdog
    vTaskDelay(pdMS_TO_TICKS(1000));
}
