#include "ringbuffer.h"

#include <cstring>
#include <Arduino.h>
#include "esp_heap_caps.h"

extern void printHeap(const char* stage);

PCMRingBuffer pcmBuffer;

bool PCMRingBuffer::begin()
{
    clear();
    return true;
}

void PCMRingBuffer::clear()
{
    head = 0;
    tail = 0;
    used = 0;
    peakUsed = 0;

    underrunCount = 0;
    overrunCount = 0;

    memset(buffer, 0, sizeof(buffer));
}

size_t PCMRingBuffer::available() const
{
    return used;
}

size_t PCMRingBuffer::free() const
{
    return (BUFFER_SAMPLES * 2) - used;
}

int PCMRingBuffer::availableForWrite()
{
    return free() * sizeof(int16_t);
}

static uint32_t s_write_cnt = 0;

static uint32_t stat_bytes_written = 0;
static uint32_t stat_bytes_read = 0;
static uint32_t stat_write_calls = 0;
static uint32_t stat_read_calls = 0;
static uint32_t last_stat_time = 0;

void print_ringbuffer_stats() {
    uint32_t now = millis();
    if (now - last_stat_time >= 1000) {
        if (last_stat_time != 0) {
            Serial.printf("\n--- THROUGHPUT STATS ---\n");
            Serial.printf("PCM Bytes Written/sec: %lu (in %lu calls)\n", stat_bytes_written, stat_write_calls);
            Serial.printf("PCM Bytes Read/sec:    %lu (in %lu calls)\n", stat_bytes_read, stat_read_calls);
            Serial.printf("Underruns: %u | Overruns: %u\n", pcmBuffer.underruns(), pcmBuffer.overruns());
            Serial.printf("Producer vs Consumer:  %s\n", (stat_bytes_written > stat_bytes_read) ? "PRODUCER FASTER" : "CONSUMER FASTER");
            Serial.printf("------------------------\n\n");
        }
        
        // Reset counters (minor race condition acceptable for rough metrics)
        stat_bytes_written = 0;
        stat_bytes_read = 0;
        stat_write_calls = 0;
        stat_read_calls = 0;
        last_stat_time = now;
    }
}

size_t PCMRingBuffer::write(const int16_t *data, size_t samples)
{
    print_ringbuffer_stats();
    
    stat_write_calls++;
    
    size_t free_space = free();
    if (samples > free_space)
    {
        samples = free_space;
        overrunCount++;
    }

    if (samples == 0) return 0;

    size_t req_bytes = samples * 2;
    constexpr size_t capacity = BUFFER_SAMPLES * 2;

    for (size_t i = 0; i < samples; i++)
    {
        buffer[head] = data[i];

        head++;

        if (head >= capacity)
            head = 0;
    }

    used += samples;
    if (used > peakUsed)
        peakUsed = used;

    stat_bytes_written += req_bytes;
    s_write_cnt++;

    return samples;
}

static uint32_t s_read_cnt = 0;

size_t PCMRingBuffer::read(int16_t *data, size_t samples)
{
    stat_read_calls++;
    
    if (samples > used)
    {
        samples = used;
        underrunCount++;
    }

    if (samples == 0) return 0;

    constexpr size_t capacity = BUFFER_SAMPLES * 2;

    for (size_t i = 0; i < samples; i++)
    {
        data[i] = buffer[tail];

        tail++;

        if (tail >= capacity)
            tail = 0;
    }

    used -= samples;

    stat_bytes_read += (samples * 2);
    s_read_cnt++;

    return samples;
}

// NEW: Read raw PCM bytes (used by Bluetooth A2DP)
size_t PCMRingBuffer::read(uint8_t *buffer, size_t bytes)
{
    // A2DP joint stereo requires L/R pairs (4 bytes per frame)
    // Enforce 4-byte alignment so we never read an odd number of samples,
    // which would completely swap L/R channels on the next read!
    size_t frame_bytes = bytes - (bytes % 4);

    size_t sampleCount = frame_bytes / sizeof(int16_t);

    size_t samplesRead =
        read(reinterpret_cast<int16_t *>(buffer), sampleCount);

    return samplesRead * sizeof(int16_t);
}

size_t PCMRingBuffer::write(uint8_t byte)
{
    return write(&byte, 1);
}

size_t PCMRingBuffer::write(const uint8_t *data, size_t size)
{
    if ((size & 1) != 0)
        return 0;

    const int16_t *samples =
        reinterpret_cast<const int16_t *>(data);

    size_t sampleCount = size / sizeof(int16_t);

    size_t writtenSamples = write(samples, sampleCount);

    return writtenSamples * sizeof(int16_t);
}

uint32_t PCMRingBuffer::underruns() const
{
    return underrunCount;
}

uint32_t PCMRingBuffer::overruns() const
{
    return overrunCount;
}

size_t PCMRingBuffer::peakFill() const
{
    return peakUsed;
}