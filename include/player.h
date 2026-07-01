#pragma once

class Player
{
public:
    void begin();

    void play();
    void pause();
    void togglePlayPause();

    void nextTrack();
    void previousTrack();

    void nextFolder();
    void previousFolder();

    void volumeUp();
    void volumeDown();

    void setVolume(int volume);

    bool isPlaying() const;
    int currentTrack() const;
    int currentFolder() const;
    int volume() const;

private:
    void printStatus();

    bool playing = false;

    int currentFolderIndex = 0;
    int currentTrackIndex = 0;

    int volumeLevel = 50;
};