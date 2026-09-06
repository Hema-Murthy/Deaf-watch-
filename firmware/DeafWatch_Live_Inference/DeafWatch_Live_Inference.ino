/*
 * DeafWatch - Real-Time Audio Scene Recognition Firmware
 * Board: ESP32-S3 / Standard ESP32
 * Microphone: INMP441 I2S Digital Microphone
 * Vibration Motor: Pin 15
 * 
 * Library Required: Exported Edge Impulse Arduino .ZIP Library
 */

#include <Arduino.h>
#include <driver/i2s.h>

// IMPORTANT: Replace this header name with your exact exported Edge Impulse library header
#include <DeafWatch-Audio-AI_inferencing.h>

// ============================================================================
// 1. Hardware Pin Definitions
// ============================================================================
#define I2S_WS_PIN        42    // LRCL / Word Select
#define I2S_SD_PIN        41    // DOUT / Data Out
#define I2S_SCK_PIN       40    // BCLK / Bit Clock
#define I2S_PORT          I2S_NUM_0

#define VIBRATION_MOTOR_PIN 15  // Transistor pin driving vibration motor
#define CONFIDENCE_THRESHOLD 0.75f // 75% confidence threshold

// Buffer for 1.0 Second Audio (16000 samples @ 16kHz)
static int16_t sampleBuffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];
static bool is_recording = false;

// ============================================================================
// 2. I2S Microphone Initialization
// ============================================================================
bool init_i2s_microphone() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK_PIN,
        .ws_io_num = I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD_PIN
    };

    if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK) {
        Serial.println("Error installing I2S driver");
        return false;
    }
    if (i2s_set_pin(I2S_PORT, &pin_config) != ESP_OK) {
        Serial.println("Error setting I2S pins");
        return false;
    }
    i2s_zero_dma_buffer(I2S_PORT);
    return true;
}

// Callback function for Edge Impulse to fetch audio samples
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    numpy::int16_to_float(sampleBuffer + offset, out_ptr, length);
    return 0;
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
    Serial.println(" ESP32-S3 Live TinyML Sound Recognition System");
    Serial.println("==================================================");

    pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);

    if (!init_i2s_microphone()) {
        Serial.println("CRITICAL: Failed to initialize INMP441 Microphone!");
        while (1);
    }
    Serial.println("INMP441 Microphone Initialized. Continuous Listening Started...");
}

void loop() {
    size_t bytes_read = 0;
    
    // Read 1 second of audio from INMP441 I2S microphone
    i2s_read(I2S_PORT, (void*)sampleBuffer, EI_CLASSIFIER_RAW_SAMPLE_COUNT * sizeof(int16_t), &bytes_read, portMAX_DELAY);

    // Prepare Edge Impulse signal wrapper
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &get_signal_data;

    // Run inference
    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR r = run_classifier(&signal, &result, false);
    if (r != EI_IMPULSE_OK) {
        Serial.printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    // Print predictions to Serial Monitor
    Serial.println("----------------------------------------");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        float score = result.classification[ix].value;
        const char* label = result.classification[ix].label;

        if (score >= CONFIDENCE_THRESHOLD && strcmp(label, "noise") != 0) {
            Serial.printf("🚨 DETECTED: %s (Confidence: %.2f%%)\n", label, score * 100.0f);
            trigger_vibration_pattern(label);
        }
    }
    
    vTaskDelay(pdMS_TO_TICKS(100)); // Yield 100ms
}
