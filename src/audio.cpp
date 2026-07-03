#include "audio.h"

#include <Arduino.h>

#include "AudioTools.h"
#include "AudioTools/AudioCodecs/AudioEncoded.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

#include "ringbuffer.h"


static audio_tools::MP3DecoderHelix mp3Decoder;
static audio_tools::EncodedAudioStream decoder(&pcmBuffer, &mp3Decoder);
static audio_tools::StreamCopy copier;

static File currentFile;

AudioEngine audio;

bool AudioEngine::begin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("       AUDIO ENGINE");
    Serial.println("==============================");

    if (!mp3Decoder.begin())
    {
        Serial.println("Helix failed to initialize.");
        return false;
    }

    decoder.begin();

    Serial.println("Helix MP3 Decoder Ready");
    Serial.println("Audio Engine Ready");
    Serial.println("==============================");

    return true;
}

bool AudioEngine::play(File file)
{
    stop();

    if (!file)
    {
        Serial.println("Audio Engine: Invalid file.");
        return false;
    }

    currentFile = file;

    copier.begin(decoder, currentFile);

    playing = true;
    paused = false;

    Serial.print("Audio Engine: Playing ");
    Serial.println(currentFile.name());

    return true;
}

void AudioEngine::stop()
{
    if (currentFile)
        currentFile.close();

    playing = false;
    paused = false;

    Serial.println("Audio Engine: Stopped");
}

void AudioEngine::pause()
{
    if (!playing || paused)
        return;

    paused = true;

    Serial.println("Audio Engine: Paused");
}

void AudioEngine::resume()
{
    if (!playing || !paused)
        return;

    paused = false;

    Serial.println("Audio Engine: Resumed");
}

bool AudioEngine::isPlaying() const
{
    return playing && !paused;
}

void AudioEngine::update()
{
    if (!playing)
        return;

    if (paused)
        return;

    size_t bytes = copier.copy();

    if (bytes == 0)
    {
        if (!currentFile.available())
        {
            stop();
            Serial.println("Audio Engine: Song Finished");
        }
    }
}