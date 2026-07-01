#include <Arduino.h>

#include "storage.h"
#include "filesystem.h"
#include "player.h"
#include "command.h"

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("===== CYBER CASSETTE =====");

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

    // Initialize Command Console
    command.begin();
}

void loop()
{
    command.update();
}