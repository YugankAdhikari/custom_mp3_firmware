#include "player.h"

#include <Arduino.h>

#include "filesystem.h"

Player player;

void Player::printStatus()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("      CYBER CASSETTE");
    Serial.println("==============================");

    Serial.print("Status : ");
    Serial.println(playing ? "Playing" : "Paused");

    Serial.print("Folder : ");
    Serial.println(filesystem.currentFolder().folderName);

    Serial.print("Track  : ");
    Serial.print(filesystem.currentTrackIndex() + 1);
    Serial.print(" / ");
    Serial.println(filesystem.currentTrackCount());

    Serial.print("Song   : ");
    Serial.println(filesystem.currentTrack().filename);

    Serial.print("Volume : ");
    Serial.print(volumeLevel);
    Serial.println("%");

    Serial.print("Loop   : ");
    Serial.println(loop ? "ON" : "OFF");

    Serial.println("==============================");
}

void Player::begin()
{
    printStatus();
}

void Player::play()
{
    playing = true;
    printStatus();
}

void Player::pause()
{
    playing = false;
    printStatus();
}

void Player::togglePlayPause()
{
    playing = !playing;
    printStatus();
}

void Player::nextTrack()
{
    filesystem.nextTrack();
    printStatus();
}

void Player::previousTrack()
{
    filesystem.previousTrack();
    printStatus();
}

void Player::nextFolder()
{
    filesystem.nextFolder();
    printStatus();
}

void Player::previousFolder()
{
    filesystem.previousFolder();
    printStatus();
}

void Player::volumeUp()
{
    if (volumeLevel < 100)
        volumeLevel++;

    printStatus();
}

void Player::volumeDown()
{
    if (volumeLevel > 0)
        volumeLevel--;

    printStatus();
}

void Player::setVolume(int volume)
{
    volumeLevel = constrain(volume, 0, 100);
}

bool Player::isPlaying() const
{
    return playing;
}

int Player::volume() const
{
    return volumeLevel;
}