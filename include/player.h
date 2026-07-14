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

    int volume() const;

    void onStreamingAvailable();

private:
    void printStatus();

    bool playing = false;
    bool pendingPlay = false;
    bool loop = false;

    int volumeLevel = 50;
};

extern Player player;