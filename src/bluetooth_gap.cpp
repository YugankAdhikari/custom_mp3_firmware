#include "bluetooth_gap.h"
#include "bluetooth_manager.h"

#include <Arduino.h>
#include <esp_log.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <esp_a2dp_api.h>

#define BT_GAP_TAG "BT_GAP"

static const char* DEVICE_NAME = "Cyber Cassette";

static DiscoveredDevice discovered_devices[MAX_DISCOVERED_DEVICES];
static int discovered_device_count = 0;

DiscoveredDevice* bt_gap_get_discovered_devices() {
    return discovered_devices;
}

int bt_gap_get_discovered_device_count() {
    return discovered_device_count;
}

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGD(BT_GAP_TAG, "GAP callback received NULL param for event: %d", event);
        return;
    }

    switch (event) {
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
            if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
                ESP_LOGI(BT_GAP_TAG, "Device discovery stopped.");
            } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
                ESP_LOGI(BT_GAP_TAG, "Device discovery started.");
                // We do NOT reset the list here in case we are stopping/starting, or users want to view it.
                // It is reset inside bt_gap_start_discovery().
            }
            break;
            
        case ESP_BT_GAP_DISC_RES_EVT: {
            // A device was found
            if (param->disc_res.prop == NULL) {
                break;
            }

            uint32_t cod = 0;
            bool is_audio_sink = false;
            char dev_name[64] = "Unknown";
            int rssi = -127;

            // Safely iterate properties, ensuring val is not NULL
            for (int i = 0; i < param->disc_res.num_prop; i++) {
                if (param->disc_res.prop[i].val == NULL) continue;

                if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_COD) {
                    cod = *(uint32_t *)(param->disc_res.prop[i].val);
                    if (esp_bt_gap_is_valid_cod(cod) && esp_bt_gap_get_cod_major_dev(cod) == ESP_BT_COD_MAJOR_DEV_AV) {
                        is_audio_sink = true;
                    }
                } else if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_BDNAME) {
                    int len = param->disc_res.prop[i].len;
                    if (len > 63) len = 63;
                    memcpy(dev_name, param->disc_res.prop[i].val, len);
                    dev_name[len] = '\0';
                } else if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_RSSI) {
                    rssi = *(int8_t *)(param->disc_res.prop[i].val);
                }
            }
            
            // Only store Audio/Video sinks
            if (is_audio_sink) {
                bool exists = false;
                for (int i = 0; i < discovered_device_count; i++) {
                    if (memcmp(discovered_devices[i].bda, param->disc_res.bda, 6) == 0) {
                        exists = true;
                        // Update RSSI and Name if valid
                        discovered_devices[i].rssi = rssi;
                        if (strcmp(dev_name, "Unknown") != 0) {
                            strcpy(discovered_devices[i].name, dev_name);
                        }
                        break;
                    }
                }

                if (!exists && discovered_device_count < MAX_DISCOVERED_DEVICES) {
                    memcpy(discovered_devices[discovered_device_count].bda, param->disc_res.bda, 6);
                    strcpy(discovered_devices[discovered_device_count].name, dev_name);
                    discovered_devices[discovered_device_count].rssi = rssi;
                    discovered_device_count++;
                }
            }
            break;
        }
            
        default:
            ESP_LOGD(BT_GAP_TAG, "Unhandled GAP event: %d", event);
            break;
    }
}

void bt_gap_setup()
{
    ESP_LOGI(BT_GAP_TAG, "Setting up GAP...");
    
    // Register GAP callback
    esp_bt_gap_register_callback(bt_app_gap_cb);
    
    // Set device name
    esp_bt_dev_set_device_name(DEVICE_NAME);
    
    // An A2DP Source should actively discover and connect to sinks.
    // It should NOT be discoverable itself while attempting discovery.
    esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
}

void bt_gap_start_discovery()
{
    ESP_LOGI(BT_GAP_TAG, "Starting GAP discovery for audio sinks...");
    discovered_device_count = 0;
    // 10 units = 12.8 seconds
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
}

void bt_gap_stop_discovery()
{
    esp_bt_gap_cancel_discovery();
}
