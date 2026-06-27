#pragma once

#include <Arduino.h>

struct PlayerState
{
    bool playing = false;

    bool bluetoothConnected = false;

    bool shuffle = false;

    bool repeat = false;

    uint8_t volume = 50;

    uint16_t currentTrack = 1;

    uint8_t currentFolder = 1;

    uint16_t totalTracks = 0;

    uint8_t totalFolders = 0;

    uint8_t batteryPercent = 100;
};

extern PlayerState playerState;