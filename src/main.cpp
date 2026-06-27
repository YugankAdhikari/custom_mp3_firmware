#include <Arduino.h>

#include "config.h"
#include "player.h"
#include "bluetooth.h"

void setup()
{
    Serial.begin(SERIAL_BAUD);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println(" Cyber Cassette");
    Serial.println("==============================");

    playerBegin();

    bluetoothBegin();

    Serial.println("System Ready");
}

void loop()
{
    if (Serial.available())
    {
        char cmd = Serial.read();

        switch (cmd)
        {
            case 'p':
                playPause();
                break;

            case 'n':
                nextTrack();
                break;

            case 'b':
                previousTrack();
                break;

            case 'f':
                nextFolder();
                break;

            case '+':
                volumeUp();
                break;

            case '-':
                volumeDown();
                break;

            case 's':
                printPlayerState();
                break;

            default:
                Serial.print("Unknown command: ");
                Serial.println(cmd);
                break;
        }
    }

    bluetoothLoop();

    delay(10);
}