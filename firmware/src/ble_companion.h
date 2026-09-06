#ifndef BLE_COMPANION_H
#define BLE_COMPANION_H

#include "config.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLECompanion : public BLEServerCallbacks {
private:
    BLEServer* pServer;
    BLECharacteristic* pAlertCharacteristic;
    BLECharacteristic* pStatusCharacteristic;
    bool deviceConnected;

public:
    BLECompanion() : pServer(nullptr), pAlertCharacteristic(nullptr), pStatusCharacteristic(nullptr), deviceConnected(false) {}

    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("[BLE] Companion App Connected!");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("[BLE] Companion App Disconnected. Restarting Advertising...");
        BLEDevice::startAdvertising();
    }

    void init() {
        BLEDevice::init(BLE_DEVICE_NAME);
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(this);

        BLEService* pService = pServer->createService(SERVICE_UUID);

        // Sound Detection Alert Characteristic (Notify enabled)
        pAlertCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID_ALERT,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
        );
        pAlertCharacteristic->addDescriptor(new BLE2902());

        // Watch System Status Characteristic
        pStatusCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID_STATUS,
            BLECharacteristic::PROPERTY_READ
        );

        pService->start();

        BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06); // functions for iPhone connection
        pAdvertising->setMinPreferred(0x12);
        BLEDevice::startAdvertising();

        Serial.println("[BLE] Service Advertising Started. Ready to pair with Custom Mobile App.");
    }

    bool isConnected() {
        return deviceConnected;
    }

    void sendSoundAlert(const char* label, float confidence) {
        if (!deviceConnected || pAlertCharacteristic == nullptr) return;

        // Build compact JSON payload for custom mobile app
        char jsonPayload[128];
        snprintf(jsonPayload, sizeof(jsonPayload), 
            "{\"event\":\"SOUND_DETECTED\",\"label\":\"%s\",\"confidence\":%.2f,\"timestamp\":%lu}",
            label, confidence, millis());

        pAlertCharacteristic->setValue((uint8_t*)jsonPayload, strlen(jsonPayload));
        pAlertCharacteristic->notify();
        Serial.printf("[BLE NOTIFY SENT] %s\n", jsonPayload);
    }
};

#endif // BLE_COMPANION_H
