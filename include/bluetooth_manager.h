#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <esp_bt_defs.h>

enum class BluetoothState {
    OFF,
    INITIALIZING,
    PAIRING,
    DISCOVERING,
    CONNECTING,
    CONNECTED,
    STREAMING,
    RECONNECTING,
    ERROR
};

class BluetoothManager
{
public:
    bool begin();
    void update();

    bool connected() const;
    bool streaming() const;
    bool pairing() const;
    bool reconnecting() const;
    BluetoothState currentState() const;

    // Internal state setters (called by GAP/A2DP logic)
    void setState(BluetoothState newState);
    
    // Command interface
    void startPairingMode();
    void triggerDiscovery();
    void connectToDevice(esp_bd_addr_t mac);
    void forgetDevice();
    void forceReconnect();

private:
    void attemptReconnect();

    BluetoothState state = BluetoothState::OFF;
    Preferences preferences;
    
    uint32_t reconnectTimer = 0;
    uint32_t reconnectInterval = 5000;
    
    esp_bd_addr_t savedMac;
    bool hasSavedMac = false;
};

extern BluetoothManager bluetooth;