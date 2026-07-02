#include "audio.h"

#include <Arduino.h>

AudioEngine audio;

bool AudioEngine::begin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("       AUDIO ENGINE");
    Serial.println("==============================");
    Serial.println("Audio Engine Ready");
    Serial.println("==============================");

    return true;
}

bool AudioEngine::play(File file)
{
    if (!file)
    {
        Serial.println("Audio: Invalid file.");
        return false;
    }

    stop();

    currentFile = file;

    playing = true;
    paused = false;

    Serial.print("Audio: Playing ");
    Serial.println(currentFile.name());

    return true;
}

void AudioEngine::stop()
{
    if (currentFile)
        currentFile.close();

    playing = false;
    paused = false;
}

void AudioEngine::pause()
{
    if (!playing)
        return;

    paused = true;

    Serial.println("Audio: Paused");
}

void AudioEngine::resume()
{
    if (!playing)
        return;

    paused = false;

    Serial.println("Audio: Resumed");
}

bool AudioEngine::isPlaying() const
{
    return playing && !paused;
}

void AudioEngine::update()
{
    // Decoder will live here later.
}