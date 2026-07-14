#pragma once

#include <Arduino.h>
#include <atomic>

class PCMRingBuffer : public Print
{
public:
    // 2048 samples * 2 channels * 2 bytes/sample = 8 KB total size.
    // At 44.1 kHz 16-bit stereo (176.4 KB/s), 8 KB provides ~46 ms of buffering latency.
    static constexpr size_t BUFFER_SAMPLES = 2048;

    bool begin();

    void clear();

    size_t available() const;
    size_t free() const;

    // AudioTools checks this before writing.
    int availableForWrite() override;

    // PCM interface
    size_t write(const int16_t *data, size_t samples);
    size_t read(int16_t *data, size_t samples);
    size_t read(uint8_t *buffer, size_t bytes);

    // Print interface
    size_t write(uint8_t byte) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    using Print::write;

    uint32_t underruns() const;
    uint32_t overruns() const;
    size_t peakFill() const;

private:
    int16_t buffer[BUFFER_SAMPLES * 2];

    size_t head = 0;
    size_t tail = 0;
    std::atomic<size_t> used{0};
    size_t peakUsed = 0;

    uint32_t underrunCount = 0;
    uint32_t overrunCount = 0;
};

extern PCMRingBuffer pcmBuffer;