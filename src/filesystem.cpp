#include "filesystem.h"

#include <Arduino.h>
#include <SD.h>
#include <cstring>

Filesystem filesystem;

static bool isMp3(const char *name)
{
    const char *dot = strrchr(name, '.');

    if (!dot)
        return false;

    return
        strcasecmp(dot, ".mp3") == 0;
}

void Filesystem::countFolders()
{
    folderCount = 0;

    File music = SD.open("/Music");

    if (!music)
        return;

    while (true)
    {
        File entry = music.openNextFile();

        if (!entry)
            break;

        if (entry.isDirectory())
            folderCount++;

        entry.close();
    }

    music.close();
}

bool Filesystem::begin()
{
    folderIndex = 0;
    trackIndex = 0;

    countFolders();

    if (folderCount == 0)
        return false;

    return loadFolder();
}

bool Filesystem::loadFolder()
{
    memset(&cache, 0, sizeof(cache));

    File music = SD.open("/Music");

    if (!music)
        return false;

    File folder;

    int current = 0;

    while (true)
    {
        folder = music.openNextFile();

        if (!folder)
        {
            music.close();
            return false;
        }

        if (!folder.isDirectory())
        {
            folder.close();
            continue;
        }

        if (current == folderIndex)
            break;

        current++;

        folder.close();
    }

    strncpy(
        cache.folderName,
        folder.name(),
        MAX_NAME_LENGTH - 1
    );

    while (true)
    {
        File file = folder.openNextFile();

        if (!file)
            break;

        if (file.isDirectory())
        {
            file.close();
            continue;
        }

        if (!isMp3(file.name()))
        {
            file.close();
            continue;
        }

        if (cache.trackCount >= MAX_TRACKS)
        {
            file.close();
            break;
        }

        strncpy(
            cache.tracks[cache.trackCount].filename,
            file.name(),
            MAX_NAME_LENGTH - 1
        );

        cache.trackCount++;

        file.close();
    }

    folder.close();
    music.close();

    if (cache.trackCount == 0)
        return false;

    if (trackIndex >= cache.trackCount)
        trackIndex = 0;

    return true;
}