#include "player.h"
#include "library.h"
#include <Arduino.h>

void Player::begin()
{
    Serial.println("Player Ready");
}

void Player::play()
{
    playing = true;
    Serial.println("[PLAYER] Play");
}

void Player::pause()
{
    playing = false;
    Serial.println("[PLAYER] Pause");
}

void Player::togglePlayPause()
{
    playing = !playing;

    Serial.println(
        playing ?
        "[PLAYER] Playing" :
        "[PLAYER] Paused");
}

void Player::nextTrack()
{
    const Folder* library = getLibrary();

    const Folder& folder = library[currentFolderIndex];

    currentTrackIndex++;

    if (currentTrackIndex >= folder.songCount)
        currentTrackIndex = 0;

    Serial.println();
    Serial.println("==============================");
    Serial.printf("Folder : %s\n", folder.name);
    Serial.printf("Track  : %d / %d\n",
                  currentTrackIndex + 1,
                  folder.songCount);
    Serial.printf("Song   : %s\n",
                  folder.songs[currentTrackIndex].title);
    Serial.println("==============================");
}

void Player::previousTrack()
{
    const Folder* library = getLibrary();

    const Folder& folder = library[currentFolderIndex];

    currentTrackIndex--;

    if (currentTrackIndex < 0)
        currentTrackIndex = folder.songCount - 1;

    Serial.println();
    Serial.println("==============================");
    Serial.printf("Folder : %s\n", folder.name);
    Serial.printf("Track  : %d / %d\n",
                  currentTrackIndex + 1,
                  folder.songCount);
    Serial.printf("Song   : %s\n",
                  folder.songs[currentTrackIndex].title);
    Serial.println("==============================");
}

void Player::nextFolder()
{
    currentFolderIndex++;

    if (currentFolderIndex >= getFolderCount())
        currentFolderIndex = 0;

    currentTrackIndex = 0;

    const Folder* library = getLibrary();

    Serial.println();
    Serial.println("==============================");
    Serial.printf("Folder : %s\n",
                  library[currentFolderIndex].name);
    Serial.printf("Track  : %d / %d\n",
                  currentTrackIndex + 1,
                  library[currentFolderIndex].songCount);
    Serial.printf("Song   : %s\n",
                  library[currentFolderIndex].songs[currentTrackIndex].title);
    Serial.println("==============================");
}

void Player::previousFolder()
{
    currentFolderIndex--;

    if (currentFolderIndex < 0)
        currentFolderIndex = getFolderCount() - 1;

    currentTrackIndex = 0;

    const Folder* library = getLibrary();

    Serial.println();
    Serial.println("==============================");
    Serial.printf("Folder : %s\n",
                  library[currentFolderIndex].name);
    Serial.printf("Track  : %d / %d\n",
                  currentTrackIndex + 1,
                  library[currentFolderIndex].songCount);
    Serial.printf("Song   : %s\n",
                  library[currentFolderIndex].songs[currentTrackIndex].title);
    Serial.println("==============================");
}

void Player::volumeUp()
{
    if (volumeLevel < 100)
        volumeLevel++;

    Serial.printf("[PLAYER] Volume %d\n", volumeLevel);
}

void Player::volumeDown()
{
    if (volumeLevel > 0)
        volumeLevel--;

    Serial.printf("[PLAYER] Volume %d\n", volumeLevel);
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