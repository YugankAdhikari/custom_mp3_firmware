#include <Arduino.h>

#include "storage.h"
#include "filesystem.h"

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("===== CYBER CASSETTE =====");

    if (!storage.begin())
    {
        Serial.println("Storage failed.");
        return;
    }

    if (!filesystem.begin())
    {
        Serial.println("Filesystem failed.");
        return;
    }

    Serial.println();
    Serial.println("Filesystem OK");
}

void loop()
{
}