#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "filesystem.h"

constexpr int SD_CS = 5;

void setup()
{
    Serial.begin(115200);

    delay(1000);

    SPI.begin(18, 19, 23, SD_CS);

    if (!SD.begin(SD_CS))
    {
        Serial.println("SD failed.");

        return;
    }

    Serial.println("SD OK");

    if (!filesystemOpenMusic())
    {
        Serial.println("Couldn't open /Music");

        return;
    }

    Serial.println();

    Serial.println("Folder:");
    Serial.println(filesystemCurrentFolder());

    Serial.println();

    Serial.println("Song:");
    Serial.println(filesystemCurrentSongName());

    Serial.println();

    Serial.println("Path:");
    Serial.println(filesystemCurrentSongPath());
}

void loop()
{
}