#include "bluetooth_manager.h"
#include "bluetooth_a2dp.h"
#include "bluetooth_gap.h"
#include "player.h"

#include <Arduino.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_a2dp_api.h>

BluetoothManager bluetooth;

bool BluetoothManager::begin()
{
    setState(BluetoothState::INITIALIZING);
    
    Serial.println();
    Serial.println("==============================");
    Serial.println("   BLUETOOTH MANAGER");
    Serial.println("==============================");
    Serial.println("Starting Bluetooth...");
    
    if (!btStart()) {
        Serial.println("Failed to initialize Bluetooth Controller (btStart)");
        setState(BluetoothState::ERROR);
        return false;
    }

    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
        if (esp_bluedroid_init() != ESP_OK) {
            Serial.println("esp_bluedroid_init failed");
            setState(BluetoothState::ERROR);
            return false;
        }
    }
    
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_INITIALIZED) {
        if (esp_bluedroid_enable() != ESP_OK) {
            Serial.println("esp_bluedroid_enable failed");
            setState(BluetoothState::ERROR);
            return false;
        }
    }

    bt_gap_setup();
    bt_a2dp_source_setup();

    preferences.begin("cyber_bt", false);
    size_t macLen = preferences.getBytes("mac", savedMac, sizeof(savedMac));
    if (macLen == 6) {
        hasSavedMac = true;
        Serial.printf("Loaded saved MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                      savedMac[0], savedMac[1], savedMac[2], savedMac[3], savedMac[4], savedMac[5]);
    } else {
        hasSavedMac = false;
        Serial.println("No saved device found.");
    }

    Serial.println("Bluetooth Initialized.");
    Serial.println("==============================");

    if (hasSavedMac) {
        setState(BluetoothState::RECONNECTING);
        reconnectInterval = 5000;
        reconnectTimer = millis() - reconnectInterval; 
    } else {
        setState(BluetoothState::OFF);
    }

    return true;
}

void BluetoothManager::update()
{
    if (state == BluetoothState::RECONNECTING) {
        if (millis() - reconnectTimer >= reconnectInterval) {
            reconnectTimer = millis();
            attemptReconnect();
            
            if (reconnectInterval < 60000) {
                reconnectInterval *= 2;
                if (reconnectInterval > 60000) reconnectInterval = 60000;
            }
        }
    }
}

void BluetoothManager::attemptReconnect()
{
    if (!hasSavedMac) {
        Serial.println("Cannot reconnect: No saved device.");
        setState(BluetoothState::OFF);
        return;
    }
    
    Serial.printf("Attempting to reconnect to: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  savedMac[0], savedMac[1], savedMac[2], savedMac[3], savedMac[4], savedMac[5]);
    
    esp_a2d_source_connect(savedMac);
}

void BluetoothManager::forceReconnect()
{
    if (hasSavedMac) {
        setState(BluetoothState::RECONNECTING);
        reconnectInterval = 5000;
        reconnectTimer = millis() - reconnectInterval; 
    } else {
        Serial.println("No device saved in NVS to reconnect to.");
    }
}

void BluetoothManager::startPairingMode()
{
    setState(BluetoothState::PAIRING);
    Serial.println("Entering Pairing Mode.");
    Serial.println("Please place your Bluetooth earbuds into pairing mode.");
    Serial.println("Press ENTER when ready.");
}

void BluetoothManager::triggerDiscovery()
{
    setState(BluetoothState::DISCOVERING);
    bt_gap_start_discovery();
}

void BluetoothManager::connectToDevice(esp_bd_addr_t mac)
{
    setState(BluetoothState::CONNECTING);
    
    preferences.putBytes("mac", mac, 6);
    memcpy(savedMac, mac, 6);
    hasSavedMac = true;
    
    Serial.printf("Connecting to selected device and saving to NVS...\n");
    esp_a2d_source_connect(mac);
}

void BluetoothManager::forgetDevice()
{
    preferences.remove("mac");
    hasSavedMac = false;
    Serial.println("Saved device forgotten.");
    if (state == BluetoothState::RECONNECTING) {
        setState(BluetoothState::OFF);
    }
}

bool BluetoothManager::connected() const { return state == BluetoothState::CONNECTED || state == BluetoothState::STREAMING; }
bool BluetoothManager::streaming() const { return state == BluetoothState::STREAMING; }
bool BluetoothManager::pairing() const { return state == BluetoothState::PAIRING || state == BluetoothState::DISCOVERING || state == BluetoothState::CONNECTING; }
bool BluetoothManager::reconnecting() const { return state == BluetoothState::RECONNECTING; }
BluetoothState BluetoothManager::currentState() const { return state; }

void BluetoothManager::setState(BluetoothState newState)
{
    if (state == newState) return;
    state = newState;
    
    Serial.printf("Bluetooth State -> %d\n", (int)state);
    
    if (state == BluetoothState::STREAMING) {
        player.onStreamingAvailable();
    }
}