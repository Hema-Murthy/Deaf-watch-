/*
 * DeafWatch - Real-Time Audio Recognition (Electret Analog Microphone Version)
 * Board: ESP32-S3 / Standard ESP32
 * Microphone: Electret Microphone (MAX4466 / MAX9814 / KY-037) on Analog Pin
 * Vibration Motor: Pin 15
 * 
 * Library Required: Exported Edge Impulse Arduino .ZIP Library
 */

#include <Arduino.h>

// IMPORTANT: Include your exported Edge Impulse header
#include <DeafWatch-Audio-AI_inferencing.h>

// ============================================================================
// 1. Hardware Pin Definitions (Electret Mic)
// ============================================================================
#define ELECTRET_MIC_ADC_PIN 1   // ESP32-S3 ADC Pin (or GPIO 34 on Standard ESP32)
#define VIBRATION_MOTOR_PIN  15  // Transistor pin driving vibration motor
#define CONFIDENCE_THRESHOLD 0.75f // 75% confidence threshold

// Buffer for 1.0 Second Audio (16000 samples @ 16kHz)
static int16_t sampleBuffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];

// Callback function for Edge Impulse to fetch audio samples
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    numpy::int16_to_float(sampleBuffer + offset, out_ptr, length);
    return 0;
}

// ============================================================================
// 2. Electret Microphone ADC Sampler (16kHz Timer Sampling)
// ============================================================================
void capture_analog_audio() {
    // Sample 16,000 readings at 16kHz (62.5 microseconds between readings)
    unsigned long sample_period_us = 62; // 1,000,000 us / 16,000 Hz ≈ 62.5 us
    
    for (int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++) {
        unsigned long start_us = micros();
        
        // Read 12-bit ADC value (0 to 4095)
        int raw_adc = analogRead(ELECTRET_MIC_ADC_PIN);
        
        // Center-align waveform around 0 (subtract 2048 DC offset) & scale to int16
        int16_t pcm_val = (int16_t)((raw_adc - 2048) * 16);
        sampleBuffer[i] = pcm_val;
        
        // Microsecond delay pacing for 16kHz sampling rate
        while ((micros() - start_us) < sample_period_us) {
            // Wait for next sampling tick
        }
    }
}

// ============================================================================
// 3. Tactile Alert Patterns
// ============================================================================
void trigger_vibration_pattern(const char* sound_label) {
    digitalWrite(VIBRATION_MOTOR_PIN, HIGH);
    if (strcmp(sound_label, "fire_alarm") == 0 || strcmp(sound_label, "sirens") == 0) {
        delay(600); // Long urgent pulse
    } else if (strcmp(sound_label, "car_horn") == 0) {
        delay(300); // Sharp pulse
    } else {
        delay(250); // Standard alert pulse
    }
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);
}

// ============================================================================
// 4. Setup & Main Loop
// ============================================================================
void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Serial.println("==================================================");
    Serial.println(" ESP32 Live Sound AI - Electret Analog Mic Mode");
    Serial.println("==================================================");

    pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);
    pinMode(ELECTRET_MIC_ADC_PIN, INPUT);

    // Optimize ADC resolution for ESP32
    analogReadResolution(12); // 0 to 4095

    Serial.println("Electret Mic Initialized. Continuous Listening Started...");
}

void loop() {
    // 1. Capture 1 second of analog audio from Electret microphone
    capture_analog_audio();

    // 2. Prepare Edge Impulse signal wrapper
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &get_signal_data;

    // 3. Run inference
    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, false);
    if (r != EI_IMPULSE_OK) {
        Serial.printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    // 4. Print predictions to Serial Monitor
    Serial.println("----------------------------------------");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        float score = result.classification[ix].value;
        const char* label = result.classification[ix].label;

        if (score >= CONFIDENCE_THRESHOLD && strcmp(label, "noise") != 0) {
            Serial.printf("🚨 DETECTED: %s (Confidence: %.2f%%)\n", label, score * 100.0f);
            trigger_vibration_pattern(label);
        }
    }
    
    delay(100);
}
