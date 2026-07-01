#pragma once

#include <Arduino.h>
#include <FS.h>

constexpr size_t MAX_NAME_LENGTH = 64;

struct Track
{
    char folder[MAX_NAME_LENGTH];
    char filename[MAX_NAME_LENGTH];
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

    int currentTrackCount() const;
    int currentFolderCount() const;

private:
    bool loadFolder();

    void countFolders();

    int folderIndex = 0;
    int trackIndex = 0;

    int folderCount = 0;
    int trackCount = 0;

    Track current{};
};

extern Filesystem filesystem;