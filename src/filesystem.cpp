#include "filesystem.h"

#include <SD.h>
#include <SPI.h>
#include <cstring>

Filesystem filesystem;

bool Filesystem::begin()
{
    folderIndex = 0;
    trackIndex = 0;
    folderCount = 0;
    trackCount = 0;

    return loadCurrentTrack();
}

bool Filesystem::nextTrack()
{
    return false;
}

bool Filesystem::previousTrack()
{
    return false;
}

bool Filesystem::nextFolder()
{
    return false;
}

bool Filesystem::previousFolder()
{
    return false;
}

const Track& Filesystem::currentTrack() const
{
    return current;
}

File Filesystem::openCurrentTrack()
{
    return File();
}

int Filesystem::currentTrackIndex() const
{
    return trackIndex;
}

int Filesystem::currentFolderIndex() const
{
    return folderIndex;
}

bool Filesystem::loadCurrentTrack()
{
    return false;
}