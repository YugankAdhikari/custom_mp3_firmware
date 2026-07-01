#include "command.h"

#include <Arduino.h>

#include "player.h"

Command command;

void Command::begin()
{
    Serial.println();
    Serial.println("===== COMMAND CONSOLE =====");
    Serial.println("Type 'h' for help.");
    Serial.println();
}

void Command::printHelp()
{
    Serial.println();
    Serial.println("========== COMMANDS ==========");
    Serial.println("h : Help");
    Serial.println("i : Player Info");
    Serial.println("p : Play / Pause");
    Serial.println("n : Next Track");
    Serial.println("b : Previous Track");
    Serial.println("f : Next Folder");
    Serial.println("r : Previous Folder");
    Serial.println("+ : Volume Up");
    Serial.println("- : Volume Down");
    Serial.println("==============================");
}

void Command::update()
{
    if (!Serial.available())
        return;

    char c = Serial.read();

    Serial.print("> ");
    Serial.println(c);

    switch (c)
    {
        case 'h':
            printHelp();
            break;

        case 'i':
            player.begin();
            break;

        case 'p':
            player.togglePlayPause();
            break;

        case 'n':
            player.nextTrack();
            break;

        case 'b':
            player.previousTrack();
            break;

        case 'f':
            player.nextFolder();
            break;

        case 'r':
            player.previousFolder();
            break;

        case '+':
            player.volumeUp();
            break;

        case '-':
            player.volumeDown();
            break;

        default:
            Serial.println("Unknown command.");
            break;
    }
}

// ======================================================
// Legacy compatibility wrappers (used by encoder.cpp)
// ======================================================

void commandRotateCW()
{
    player.volumeUp();
}

void commandRotateCCW()
{
    player.volumeDown();
}

void commandClick()
{
    player.nextFolder();
}

void commandLongClick()
{
    player.togglePlayPause();
}