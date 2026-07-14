#include "audio.h"

#include <Arduino.h>
#include "esp_heap_caps.h"

void printHeap(const char* stage)
{
    Serial.printf(
        "\n===== %s =====\n"
        "Free Heap      : %u\n"
        "Largest Block  : %u\n"
        "Min Free Heap  : %u\n"
        "Internal Heap  : %u\n"
        "====================\n",
        stage,
        ESP.getFreeHeap(),
        heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
        ESP.getMinFreeHeap(),
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL)
    );
}

#include "CompatibilityPlaybackAdapter.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/AudioEncoded.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

#include "ringbuffer.h"

static audio_tools::MP3DecoderHelix mp3Decoder;
static CompatibilityPlaybackAdapter pcmConverter(pcmBuffer);
static audio_tools::EncodedAudioStream decoder(&pcmConverter, &mp3Decoder);

class FormatUpdater : public audio_tools::AudioInfoSupport {
public:
    void setAudioInfo(audio_tools::AudioInfo info) override {
        static bool firstLog = true;
        
        pcmConverter.setAudioInfo(info);
        audio_tools::AudioInfo targetInfo = pcmConverter.audioInfoOut();
        
        if (firstLog) {
            Serial.println();
            Serial.println("==============================");
            Serial.println("    FORMAT CONVERTER INIT");
            Serial.println("==============================");
            Serial.printf("Input:  %d Hz, %d ch, %d bit\n", info.sample_rate, info.channels, info.bits_per_sample);
            Serial.printf("Output: %d Hz, %d ch, %d bit\n", targetInfo.sample_rate, targetInfo.channels, targetInfo.bits_per_sample);
            Serial.println("==============================\n");
            firstLog = false;
        }
        
        printHeap("Before pcmConverter.begin()");
        pcmConverter.begin();
        printHeap("After pcmConverter.begin()");
    }

    audio_tools::AudioInfo audioInfo() override {
        return pcmConverter.audioInfoOut();
    }
};

static FormatUpdater formatUpdater;
static audio_tools::StreamCopy copier;

static File currentFile;

AudioEngine audio;

bool AudioEngine::begin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("       AUDIO ENGINE");
    Serial.printf("pcmBuffer address (audio): %p\n", &pcmBuffer);
    Serial.println("==============================");

    printHeap("Before mp3Decoder.begin()");
    if (!mp3Decoder.begin())
    {
        Serial.println("Helix failed to initialize.");
        return false;
    }
    printHeap("After mp3Decoder.begin()");

    decoder.addNotifyAudioChange(formatUpdater);
    
    printHeap("Before decoder.begin()");
    decoder.begin();
    printHeap("After decoder.begin()");

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

    // Check for ID3v2 tag and skip it to prevent the MP3 decoder from 
    // interpreting album art as audio (which causes horrible choppiness at the start)
    uint8_t id3_header[10];
    if (currentFile.read(id3_header, 10) == 10) {
        if (id3_header[0] == 'I' && id3_header[1] == 'D' && id3_header[2] == '3') {
            // It's an ID3v2 tag. Calculate size.
            // Size is 4 bytes, 7 bits per byte (syncsafe integer)
            uint32_t tag_size = (id3_header[6] << 21) | (id3_header[7] << 14) | (id3_header[8] << 7) | id3_header[9];
            Serial.printf("Audio Engine: Skipping ID3 tag of size %lu bytes\n", tag_size);
            currentFile.seek(10 + tag_size);
        } else {
            // Not an ID3 tag, rewind to start
            currentFile.seek(0);
        }
    } else {
        currentFile.seek(0);
    }

    printHeap("Before copier.begin(decoder, currentFile)");
    copier.begin(decoder, currentFile);
    printHeap("After copier.begin(decoder, currentFile)");

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

    static bool infoPrinted = false;
    if (!infoPrinted && pcmBuffer.available() > 0)
    {
        auto info = mp3Decoder.audioInfo();
        Serial.println("\n--- HELIX DECODER OUTPUT INFO ---");
        Serial.printf("Sample Rate: %d Hz\n", info.sample_rate);
        Serial.printf("Channels: %d\n", info.channels);
        Serial.printf("Bits per sample: %d\n", info.bits_per_sample);
        Serial.println("---------------------------------\n");
        
        // Delay for 3 seconds to ensure the user can read this block before the console scrolls
        delay(3000);
        
        infoPrinted = true;
    }

    size_t bytes = copier.copy();

    if (bytes == 0)
    {
        if (!currentFile.available())
        {
            stop();
            Serial.println("Audio Engine: Song Finished");
            infoPrinted = false; // reset for next song
        }
    }
}