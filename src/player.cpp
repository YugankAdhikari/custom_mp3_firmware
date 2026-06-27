#include <Arduino.h>
#include "player.h"

static bool playing = false;

void playerBegin()
{
    Serial.println("Player initialized");
}

void playPause()
{
    playing = !playing;

    Serial.println(
        playing ?
        "Playing" :
        "Paused");
}

void nextTrack()
{
    Serial.println("Next Track");
}

void previousTrack()
{
    Serial.println("Previous Track");
}

void nextFolder()
{
    Serial.println("Next Folder");
}

void volumeUp()
{
    Serial.println("Volume +");
}

void volumeDown()
{
    Serial.println("Volume -");
}

void printPlayerState()
{
    Serial.print("Playing: ");
    Serial.println(playing);
}