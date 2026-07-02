#pragma once

#include <FS.h>

class AudioEngine
{
public:
    bool begin();

    bool play(File file);

    void stop();

    void pause();

    void resume();

    bool isPlaying() const;

    void update();

private:
    File currentFile;

    bool playing = false;
    bool paused = false;
};

extern AudioEngine audio;