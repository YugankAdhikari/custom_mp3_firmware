#include <Arduino.h>
#include "command.h"
#include "player.h"

extern Player player;

String input;

void commandBegin()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("      Cyber Cassette v0.1");
    Serial.println("Type 'help' to begin.");
    Serial.println("=================================");
    Serial.println();
}

void commandLoop()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n' || c == '\r')
        {
            input.trim();

            if (input.length() > 0)
            {
                Serial.print("> ");
                Serial.println(input);

                if (input == "next")
                    player.nextTrack();

                else if (input == "prev")
                    player.previousTrack();

                else if (input == "folder+")
                    player.nextFolder();

                else if (input == "folder-")
                    player.previousFolder();

                else if (input == "play")
                    player.play();

                else if (input == "pause")
                    player.pause();

                else if (input == "help")
                {
                    Serial.println();
                    Serial.println("Commands:");
                    Serial.println(" play");
                    Serial.println(" pause");
                    Serial.println(" next");
                    Serial.println(" prev");
                    Serial.println(" folder+");
                    Serial.println(" folder-");
                    Serial.println();
                }
                else
                {
                    Serial.println("Unknown command.");
                }

                input = "";
            }
        }
        else
        {
            input += c;
        }
    }
}