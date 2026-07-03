
#include "bluetooth.h"

#include <Arduino.h>

#include "BluetoothA2DPSource.h"
#include "ringbuffer.h"

static BluetoothA2DPSource a2dp;

static int32_t get_data(uint8_t *data, int32_t len)
{
    return pcmBuffer.read(data, len);
}

bool bluetoothBegin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("       BLUETOOTH");
    Serial.println("==============================");

    a2dp.set_data_callback(get_data);

    Serial.printf("Free heap before BT: %u\n", ESP.getFreeHeap());

    Serial.printf("Core ID: %d\n", xPortGetCoreID());
    Serial.printf("CPU count: %d\n", portNUM_PROCESSORS);
    a2dp.set_task_core(1);
    a2dp.start("Cyber Cassette");
    Serial.printf("Free heap after BT: %u\n", ESP.getFreeHeap());

    Serial.println("Bluetooth A2DP Source Started");
    Serial.println("Pair your earbuds with:");
    Serial.println("Cyber Cassette");
    Serial.println("==============================");

    return true;
}

void bluetoothLoop()
{
    // Reserved for future Bluetooth event handling.
}