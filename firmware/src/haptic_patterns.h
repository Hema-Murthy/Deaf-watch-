#ifndef HAPTIC_PATTERNS_H
#define HAPTIC_PATTERNS_H

#include "config.h"

class HapticFeedback {
private:
    uint8_t pin;

    void pulse(uint32_t duration_ms) {
        digitalWrite(pin, HIGH);
        delay(duration_ms);
        digitalWrite(pin, LOW);
    }

public:
    HapticFeedback(uint8_t motor_pin = VIBRATION_MOTOR_PIN) : pin(motor_pin) {}

    void init() {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // Trigger unique vibration sequences based on recognized sound class
    void trigger_alert(const char* label) {
        Serial.printf("[HAPTIC ALERT] Triggering pattern for: %s\n", label);

        if (strcmp(label, "fire_alarm") == 0 || strcmp(label, "sirens") == 0) {
            // Urgent continuous rapid pulses (Emergency Tier 1)
            for (int i = 0; i < 6; i++) {
                pulse(120);
                delay(60);
            }
        } 
        else if (strcmp(label, "car_horn") == 0) {
            // Sharp triple burst (Roadway Hazard Tier 2)
            for (int i = 0; i < 3; i++) {
                pulse(250);
                delay(100);
            }
        } 
        else if (strcmp(label, "doorbell_knock") == 0) {
            // Double heavy pulse (Domestic Visitor)
            pulse(400);
            delay(150);
            pulse(400);
        } 
        else if (strcmp(label, "baby_crying") == 0) {
            // Rhythmic long-short wave (Caregiver Alert)
            pulse(300);
            delay(200);
            pulse(150);
            delay(200);
            pulse(300);
        } 
        else if (strcmp(label, "alarm") == 0) {
            // Continuous repeating alarm pulse (Wake-up / Reminder Alarm)
            for (int i = 0; i < 4; i++) {
                pulse(200);
                delay(100);
            }
        }
        // "noise" baseline does NOT trigger haptic feedback
    }
};

#endif // HAPTIC_PATTERNS_H
