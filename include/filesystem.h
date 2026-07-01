#pragma once

#include <Arduino.h>
#include <FS.h>

constexpr size_t MAX_NAME_LENGTH = 64;
constexpr int MAX_TRACKS = 512;

struct Track
{
    char filename[MAX_NAME_LENGTH];
};

struct FolderCache
{
    char folderName[MAX_NAME_LENGTH];
    Track tracks[MAX_TRACKS];
    int trackCount = 0;
};

class Filesystem
{
public:
    bool begin();

    bool nextTrack();
    bool previousTrack();

    bool nextFolder();
    bool previousFolder();

    const FolderCache& currentFolder() const;
    const Track& currentTrack() const;

    File openCurrentTrack();

    int currentTrackIndex() const;
    int currentFolderIndex() const;

    int currentTrackCount() const;
    int currentFolderCount() const;

private:
    bool loadFolder();

    void countFolders();

    FolderCache cache;

    int folderIndex = 0;
    int trackIndex = 0;

    int folderCount = 0;
};

extern Filesystem filesystem;