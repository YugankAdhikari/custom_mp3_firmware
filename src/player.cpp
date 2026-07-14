#include "player.h"

#include <Arduino.h>

#include "filesystem.h"
#include "audio.h"
#include "bluetooth_manager.h"

Player player;

void Player::printStatus()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("      CYBER CASSETTE");
    Serial.println("==============================");

    Serial.print("Status : ");
    if (pendingPlay) {
        Serial.println("Pending Bluetooth Stream");
    } else {
        Serial.println(audio.isPlaying() ? "Playing" : "Paused");
    }

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
    if (audio.isPlaying() && !pendingPlay)
        return;

    playing = true;

    if (!bluetooth.streaming()) {
        pendingPlay = true;
        Serial.println("Playback requested. Waiting for Bluetooth STREAMING state...");
    } else {
        pendingPlay = false;
        audio.play(filesystem.openCurrentTrack());
    }

    printStatus();
}

void Player::pause()
{
    pendingPlay = false;
    
    if (!playing)
        return;

    audio.pause();
    playing = false;

    printStatus();
}

void Player::togglePlayPause()
{
    if (playing)
    {
        pause();
    }
    else
    {
        play();
    }
}

void Player::onStreamingAvailable()
{
    if (pendingPlay && playing) {
        Serial.println("Bluetooth STREAMING available. Automatically resuming playback.");
        pendingPlay = false;
        audio.play(filesystem.openCurrentTrack());
        printStatus();
    }
}

void Player::nextTrack()
{
    if (!filesystem.nextTrack())
        return;

    if (playing) {
        if (!bluetooth.streaming()) {
            pendingPlay = true;
        } else {
            pendingPlay = false;
            audio.play(filesystem.openCurrentTrack());
        }
    }

    printStatus();
}

void Player::previousTrack()
{
    if (!filesystem.previousTrack())
        return;

    if (playing) {
        if (!bluetooth.streaming()) {
            pendingPlay = true;
        } else {
            pendingPlay = false;
            audio.play(filesystem.openCurrentTrack());
        }
    }

    printStatus();
}

void Player::nextFolder()
{
    if (!filesystem.nextFolder())
        return;

    if (playing) {
        if (!bluetooth.streaming()) {
            pendingPlay = true;
        } else {
            pendingPlay = false;
            audio.play(filesystem.openCurrentTrack());
        }
    }

    printStatus();
}

void Player::previousFolder()
{
    if (!filesystem.previousFolder())
        return;

    if (playing) {
        if (!bluetooth.streaming()) {
            pendingPlay = true;
        } else {
            pendingPlay = false;
            audio.play(filesystem.openCurrentTrack());
        }
    }

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