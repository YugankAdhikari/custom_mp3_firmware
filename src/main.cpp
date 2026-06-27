#include <Arduino.h>
#include "player.h"
#include "command.h"

Player player;

void setup()
{
    Serial.begin(115200);

    player.begin();

    commandBegin();
}

void loop()
{
    commandLoop();
}