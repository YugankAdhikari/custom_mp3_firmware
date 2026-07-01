#pragma once

#include <Arduino.h>
#include <FS.h>

struct Track
{
    char folder[64];
    char filename[128];
};

class Filesystem
{
public:
    bool begin();

    bool nextTrack();
    bool previousTrack();

    bool nextFolder();
    bool previousFolder();

    const Track& currentTrack() const;

    File openCurrentTrack();

    int currentTrackIndex() const;
    int currentFolderIndex() const;

private:
    bool loadCurrentTrack();

    int folderIndex = 0;
    int trackIndex = 0;

    Track current;
};

extern Filesystem filesystem;