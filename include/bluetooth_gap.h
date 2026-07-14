#pragma once

#include <esp_bt_defs.h>

#define MAX_DISCOVERED_DEVICES 10

struct DiscoveredDevice {
    esp_bd_addr_t bda;
    char name[64];
    int rssi;
};

// Set up GAP callbacks, device name, and scan mode
void bt_gap_setup();

// Start discovering Bluetooth audio sinks
void bt_gap_start_discovery();

// Stop discovery
void bt_gap_stop_discovery();

// Retrieve discovered devices
DiscoveredDevice* bt_gap_get_discovered_devices();
int bt_gap_get_discovered_device_count();
