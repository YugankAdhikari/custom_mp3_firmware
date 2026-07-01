#include "player.h"
#include "filesystem.h"
#include <Arduino.h>

void Player::printStatus()
{
    const Folder* library = getLibrary();
    const Folder& folder = library[currentFolderIndex];

    Serial.println();
    Serial.println("==============================");
    Serial.println("      CYBER CASSETTE");
    Serial.println("==============================");

    Serial.printf("Status : %s\n",
                  playing ? "Playing" : "Paused");

    Serial.printf("Folder : %s\n",
                  folder.name);

    Serial.printf("Track  : %d / %d\n",
                  currentTrackIndex + 1,
                  folder.songCount);

    Serial.printf("Song   : %s\n",
                  folder.songs[currentTrackIndex].title);

    Serial.printf("Volume : %d%%\n",
                  volumeLevel);

    Serial.println("==============================");
}

void Player::begin()
{
    Serial.println("Player Ready");
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
    const Folder* library = getLibrary();
    const Folder& folder = library[currentFolderIndex];

    currentTrackIndex++;

    if (currentTrackIndex >= folder.songCount)
        currentTrackIndex = 0;

    printStatus();
}

void Player::previousTrack()
{
    const Folder* library = getLibrary();
    const Folder& folder = library[currentFolderIndex];

    currentTrackIndex--;

    if (currentTrackIndex < 0)
        currentTrackIndex = folder.songCount - 1;

    printStatus();
}

void Player::nextFolder()
{
    currentFolderIndex++;

    if (currentFolderIndex >= getFolderCount())
        currentFolderIndex = 0;

    currentTrackIndex = 0;

    printStatus();
}

void Player::previousFolder()
{
    currentFolderIndex--;

    if (currentFolderIndex < 0)
        currentFolderIndex = getFolderCount() - 1;

    currentTrackIndex = 0;

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

int Player::currentTrack() const
{
    return currentTrackIndex;
}

int Player::currentFolder() const
{
    return currentFolderIndex;
}

int Player::volume() const
{
    return volumeLevel;
}