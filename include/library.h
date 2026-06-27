#pragma once

#include <Arduino.h>

struct Song
{
    const char* title;
};

struct Folder
{
    const char* name;

    const Song* songs;

    int songCount;
};

const Folder* getLibrary();

int getFolderCount();