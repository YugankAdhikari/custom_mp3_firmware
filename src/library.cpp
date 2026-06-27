#include "library.h"

static const Song chillSongs[] =
{
    {"Night Drive"},
    {"Neon Rain"},
    {"Midnight City"}
};

static const Song lofiSongs[] =
{
    {"Coffee Shop"},
    {"Sunday Morning"},
    {"Clouds"}
};

static const Song rockSongs[] =
{
    {"Thunder"},
    {"Firestorm"},
    {"Electric Heart"}
};

static const Folder library[] =
{
    {
        "Chill",
        chillSongs,
        3
    },

    {
        "Lofi",
        lofiSongs,
        3
    },

    {
        "Rock",
        rockSongs,
        3
    }
};

const Folder* getLibrary()
{
    return library;
}

int getFolderCount()
{
    return sizeof(library)/sizeof(Folder);
}