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

    return strcasecmp(dot, ".mp3") == 0;
}

static const char* baseName(const char* path)
{
    const char* slash = strrchr(path, '/');

    if (slash)
        return slash + 1;

    return path;
}

void Filesystem::countFolders()
{
    folderCount = 0;

    File music = SD.open("/Music");

    if (!music)
    {
        Serial.println("Couldn't open /Music");
        return;
    }

    Serial.println("Counting folders...");

    while (true)
    {
        File entry = music.openNextFile();

        if (!entry)
            break;

        const char* name = baseName(entry.name());

        Serial.print("Found: ");
        Serial.print(name);

        if (!entry.isDirectory())
        {
            Serial.println("  [FILE]");
            entry.close();
            continue;
        }

        if (name[0] == '.')
        {
            Serial.println("  [HIDDEN]");
            entry.close();
            continue;
        }

        folderCount++;

        Serial.println("  [DIR]");

        entry.close();
    }

    music.close();

    Serial.print("Folder count = ");
    Serial.println(folderCount);
}

bool Filesystem::begin()
{
    Serial.println();
    Serial.println("========== FILESYSTEM ==========");

    folderIndex = 0;
    trackIndex = 0;

    countFolders();

    if (folderCount == 0)
    {
        Serial.println("No music folders found.");
        return false;
    }

    return loadFolder();
}

bool Filesystem::loadFolder()
{
    memset(&cache, 0, sizeof(cache));

    File music = SD.open("/Music");

    if (!music)
    {
        Serial.println("Couldn't reopen /Music");
        return false;
    }

    Serial.println("Opened /Music");

    File folder;

    int current = 0;

    while (true)
    {
        folder = music.openNextFile();

        if (!folder)
        {
            Serial.println("No folder selected.");
            music.close();
            return false;
        }

        const char* folderName = baseName(folder.name());

        Serial.print("Examining: ");
        Serial.println(folderName);

        if (!folder.isDirectory())
        {
            folder.close();
            continue;
        }

        if (folderName[0] == '.')
        {
            Serial.println(" -> Hidden folder, skipping");
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
        baseName(folder.name()),
        MAX_NAME_LENGTH - 1
    );

    cache.folderName[MAX_NAME_LENGTH - 1] = '\0';

    Serial.print("Selected folder: ");
    Serial.println(cache.folderName);

    while (true)
    {
        File file = folder.openNextFile();

        if (!file)
            break;

        const char* fileName = baseName(file.name());

        Serial.print("Checking: ");
        Serial.println(fileName);

        if (file.isDirectory())
        {
            file.close();
            continue;
        }

        if (fileName[0] == '.')
        {
            Serial.println(" -> Hidden file");
            file.close();
            continue;
        }

        if (!isMp3(fileName))
        {
            Serial.println(" -> Not an MP3");
            file.close();
            continue;
        }

        if (cache.trackCount >= MAX_TRACKS)
        {
            Serial.println("Track cache full.");
            file.close();
            break;
        }

        strncpy(
            cache.tracks[cache.trackCount].filename,
            fileName,
            MAX_NAME_LENGTH - 1
        );

        cache.tracks[cache.trackCount].filename[MAX_NAME_LENGTH - 1] = '\0';

        Serial.print("Added MP3: ");
        Serial.println(cache.tracks[cache.trackCount].filename);

        cache.trackCount++;

        file.close();
    }

    folder.close();
    music.close();

    Serial.print("Tracks loaded: ");
    Serial.println(cache.trackCount);

    if (cache.trackCount == 0)
    {
        Serial.println("No MP3 files found.");
        return false;
    }

    if (trackIndex >= cache.trackCount)
        trackIndex = 0;

    Serial.println("Filesystem ready.");

    return true;
}