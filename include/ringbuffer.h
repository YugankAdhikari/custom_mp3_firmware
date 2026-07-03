#pragma once

#include <Arduino.h>

class PCMRingBuffer : public Print
{
public:
    static constexpr size_t BUFFER_SAMPLES = 8192;

    bool begin();

    void clear();

    size_t available() const;
    size_t free() const;

    // AudioTools checks this before writing.
    int availableForWrite() override;

    // PCM interface
    bool write(const int16_t *data, size_t samples);
    size_t read(int16_t *data, size_t samples);
    size_t read(uint8_t *buffer, size_t bytes);

    // Print interface
    size_t write(uint8_t byte) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    using Print::write;

    uint32_t underruns() const;
    uint32_t overruns() const;

private:
    int16_t buffer[BUFFER_SAMPLES * 2];

    size_t head = 0;
    size_t tail = 0;
    size_t used = 0;

    uint32_t underrunCount = 0;
    uint32_t overrunCount = 0;
};

extern PCMRingBuffer pcmBuffer;