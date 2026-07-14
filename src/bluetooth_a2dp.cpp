#include "bluetooth_a2dp.h"
#include "bluetooth_task.h"
#include "ringbuffer.h"
#include "bluetooth_manager.h"

#include <esp_log.h>
#include <esp_a2dp_api.h>

#define BT_A2DP_TAG "BT_A2DP"

static void bt_app_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    ESP_LOGI(BT_A2DP_TAG, "A2DP Callback Event: %d", event);
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            ESP_LOGI(BT_A2DP_TAG, "A2DP connection state: %d", param->conn_stat.state);
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                bluetooth.setState(BluetoothState::CONNECTED);
                ESP_LOGI(BT_A2DP_TAG, "A2DP Connected!");
                // The Source must explicitly start the streaming session.
                esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                if (bluetooth.pairing()) {
                    ESP_LOGI(BT_A2DP_TAG, "Connection failed during pairing mode. Please try again.");
                } else {
                    bluetooth.forceReconnect();
                }
                ESP_LOGI(BT_A2DP_TAG, "A2DP Disconnected!");
            }
            break;
        case ESP_A2D_AUDIO_STATE_EVT:
            ESP_LOGI(BT_A2DP_TAG, "A2DP audio state: %d", param->audio_stat.state);
            if (param->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
                bluetooth.setState(BluetoothState::STREAMING);
            } else if (param->audio_stat.state == ESP_A2D_AUDIO_STATE_STOPPED) {
                if (bluetooth.currentState() == BluetoothState::STREAMING) {
                    bluetooth.setState(BluetoothState::CONNECTED);
                }
            }
            break;
        case ESP_A2D_AUDIO_CFG_EVT: {
            ESP_LOGI(BT_A2DP_TAG, "A2DP audio cfg");
            
            if (param->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
                uint8_t *sbc = param->audio_cfg.mcc.cie.sbc;
                ESP_LOGI(BT_A2DP_TAG, "--- A2DP SBC NEGOTIATED CONFIG ---");
                
                // Byte 0: Sample Rate (bits 7-4)
                if (sbc[0] & 0x80) ESP_LOGI(BT_A2DP_TAG, "Sample Rate: 16000 Hz");
                if (sbc[0] & 0x40) ESP_LOGI(BT_A2DP_TAG, "Sample Rate: 32000 Hz");
                if (sbc[0] & 0x20) ESP_LOGI(BT_A2DP_TAG, "Sample Rate: 44100 Hz");
                if (sbc[0] & 0x10) ESP_LOGI(BT_A2DP_TAG, "Sample Rate: 48000 Hz");
                
                // Byte 0: Channel Mode (bits 3-0)
                if (sbc[0] & 0x08) ESP_LOGI(BT_A2DP_TAG, "Channels: Mono");
                if (sbc[0] & 0x04) ESP_LOGI(BT_A2DP_TAG, "Channels: Dual Channel");
                if (sbc[0] & 0x02) ESP_LOGI(BT_A2DP_TAG, "Channels: Stereo");
                if (sbc[0] & 0x01) ESP_LOGI(BT_A2DP_TAG, "Channels: Joint Stereo");
                
                ESP_LOGI(BT_A2DP_TAG, "Bitpool Min: %d, Max: %d", sbc[2], sbc[3]);
                ESP_LOGI(BT_A2DP_TAG, "----------------------------------");
            }
            break;
        }
        case ESP_A2D_MEDIA_CTRL_ACK_EVT:
            ESP_LOGI(BT_A2DP_TAG, "A2DP media ctrl ack: cmd=%d, status=%d", param->media_ctrl_stat.cmd, param->media_ctrl_stat.status);
            if (param->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_START) {
                if (param->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
                    ESP_LOGI(BT_A2DP_TAG, "A2DP media start >>> ACKNOWLEDGED SUCCESS <<<");
                } else {
                    ESP_LOGI(BT_A2DP_TAG, "A2DP media start >>> REJECTED OR FAILED <<<");
                }
            }
            break;
        default:
            ESP_LOGI(BT_A2DP_TAG, "A2DP unhandled event: %d", event);
            break;
    }
}

static uint32_t s_cb_count = 0;

static int32_t bt_app_a2d_data_cb(uint8_t *data, int32_t len)
{
    if (len <= 0 || data == nullptr) {
        return 0;
    }

    // A2DP requires full chunks (usually 512 bytes) to encode SBC frames smoothly.
    // If we don't have enough data, we return 0. This tells the ESP-IDF stack to wait 
    // for the next tick, avoiding partial frame drops or injecting silence.
    if (pcmBuffer.available() * sizeof(int16_t) < (size_t)len) {
        return 0;
    }

    size_t read_bytes = pcmBuffer.read(data, (size_t)len);
    
    return (int32_t)read_bytes;
}

void bt_a2dp_source_setup()
{
    ESP_LOGI(BT_A2DP_TAG, "Setting up A2DP Source callbacks...");
    ESP_LOGI(BT_A2DP_TAG, "pcmBuffer address (bt): %p", &pcmBuffer);
    
    // Register the connection/audio state callback
    esp_a2d_register_callback(bt_app_a2d_cb);
    
    // Register the data source callback for SBC encoding
    esp_a2d_source_register_data_callback(bt_app_a2d_data_cb);
    
    // Initialize the A2DP Source profile
    esp_a2d_source_init();
}
