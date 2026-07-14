#include <Arduino.h>

#include "storage.h"
#include "filesystem.h"
#include "player.h"
#include "command.h"
#include "audio.h"
#include "ringbuffer.h"
#include "bluetooth_manager.h"

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("===== CYBER CASSETTE =====");

    // Initialize Bluetooth BEFORE storage/audio to ensure maximum heap for BT tasks
    if (!bluetooth.begin())
    {
        Serial.println("Bluetooth failed.");
        return;
    }

    // Initialize Storage
    if (!storage.begin())
    {
        Serial.println("Storage failed.");
        return;
    }

    // Initialize Filesystem
    if (!filesystem.begin())
    {
        Serial.println("Filesystem failed.");
        return;
    }

    Serial.println();
    Serial.println("Filesystem OK");

    // Initialize Player
    player.begin();

    // Initialize Audio Engine
    if (!audio.begin())
    {
        Serial.println("Audio Engine failed.");
        return;
    }

    // Initialize PCM Ring Buffer
    if (!pcmBuffer.begin())
    {
        Serial.println("Ring Buffer failed.");
        return;
    }

    Serial.println("PCM Ring Buffer Ready");

    // Initialize Command Console
    command.begin();

    Serial.println();
    Serial.println("===== SYSTEM READY =====");
}

void loop()
{
    command.update();

    audio.update();

    bluetooth.update();
}