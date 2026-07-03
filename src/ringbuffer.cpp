#include "ringbuffer.h"

#include <cstring>

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

bool PCMRingBuffer::write(const int16_t *data, size_t samples)
{
    if (samples > free())
    {
        overrunCount++;
        return false;
    }

    constexpr size_t capacity = BUFFER_SAMPLES * 2;

    for (size_t i = 0; i < samples; i++)
    {
        buffer[head] = data[i];

        head++;

        if (head >= capacity)
            head = 0;
    }

    used += samples;

    return true;
}

size_t PCMRingBuffer::read(int16_t *data, size_t samples)
{
    if (samples > used)
    {
        samples = used;
        underrunCount++;
    }

    constexpr size_t capacity = BUFFER_SAMPLES * 2;

    for (size_t i = 0; i < samples; i++)
    {
        data[i] = buffer[tail];

        tail++;

        if (tail >= capacity)
            tail = 0;
    }

    used -= samples;

    return samples;
}

// NEW: Read raw PCM bytes (used by Bluetooth A2DP)
size_t PCMRingBuffer::read(uint8_t *buffer, size_t bytes)
{
    // Make sure we're reading whole 16-bit samples
    if (bytes & 1)
        bytes--;

    size_t sampleCount = bytes / sizeof(int16_t);

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

    if (!write(samples, sampleCount))
        return 0;

    return size;
}

uint32_t PCMRingBuffer::underruns() const
{
    return underrunCount;
}

uint32_t PCMRingBuffer::overruns() const
{
    return overrunCount;
}