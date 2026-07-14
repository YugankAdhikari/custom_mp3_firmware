#pragma once

#include <Arduino.h>
#include "AudioTools/CoreAudio/AudioStreams.h"
#include "ringbuffer.h"

class CompatibilityPlaybackAdapter : public audio_tools::AudioOutput {
public:
    CompatibilityPlaybackAdapter(PCMRingBuffer& buffer) : _pcmBuffer(buffer) {
        _info.sample_rate = 16000;
        _info.channels = 1;
        _info.bits_per_sample = 16;
        _phase_step = 16000.0f / 44100.0f;
    }

    void setAudioInfo(audio_tools::AudioInfo info) override {
        if (info.sample_rate == 0 || info.channels == 0) return;
        
        _info = info;
        _phase_step = (float)_info.sample_rate / 44100.0f;
        _last_L = 0;
        _last_R = 0;
        _phase = 0.0f;
        
        audio_tools::AudioOutput::setAudioInfo(info);
    }

    audio_tools::AudioInfo audioInfoOut() {
        audio_tools::AudioInfo out = _info;
        out.sample_rate = 44100;
        out.channels = 2; // Always stereo output
        out.bits_per_sample = 16;
        return out;
    }

    size_t write(const uint8_t* data, size_t len) override {
        if (len == 0 || data == nullptr) return 0;
        
        size_t input_samples = len / sizeof(int16_t);
        size_t input_frames = input_samples / _info.channels;
        const int16_t* in_data = (const int16_t*)data;
        
        const size_t BATCH_FRAMES = 128;
        int16_t out_batch[BATCH_FRAMES * 2]; // 2 channels
        size_t batch_idx = 0;

        size_t frames_consumed = 0;

        auto flush = [&]() {
            if (batch_idx == 0) return;
            size_t bytes_to_write = batch_idx * 2 * sizeof(int16_t);
            const uint8_t* ptr = (const uint8_t*)out_batch;
            while (bytes_to_write > 0) {
                size_t written = _pcmBuffer.write(ptr, bytes_to_write);
                if (written > 0) {
                    ptr += written;
                    bytes_to_write -= written;
                } else {
                    delay(2); // Yield CPU to allow Bluetooth task to consume
                }
            }
            batch_idx = 0;
        };

        for (size_t i = 0; i < input_frames; i++) {
            int16_t curr_L, curr_R;
            if (_info.channels == 1) {
                curr_L = in_data[i];
                curr_R = curr_L;
            } else {
                curr_L = in_data[i * 2];
                curr_R = in_data[i * 2 + 1];
            }

            // If input is already 44.1 kHz, we can skip interpolation math
            if (abs(_phase_step - 1.0f) < 0.001f) {
                if (batch_idx >= BATCH_FRAMES) {
                    flush();
                }
                out_batch[batch_idx * 2] = curr_L;
                out_batch[batch_idx * 2 + 1] = curr_R;
                batch_idx++;
            } else {
                // Linear Interpolation Resampling
                while (_phase < 1.0f) {
                    if (batch_idx >= BATCH_FRAMES) {
                        flush();
                    }
                    int16_t out_L = _last_L + (int16_t)((curr_L - _last_L) * _phase);
                    int16_t out_R = _last_R + (int16_t)((curr_R - _last_R) * _phase);
                    
                    out_batch[batch_idx * 2] = out_L;
                    out_batch[batch_idx * 2 + 1] = out_R;
                    batch_idx++;
                    
                    _phase += _phase_step;
                }
                _phase -= 1.0f;
            }

            _last_L = curr_L;
            _last_R = curr_R;
            frames_consumed++;
        }
        
        // Flush whatever remains.
        flush();
        
        return frames_consumed * _info.channels * sizeof(int16_t);
    }

    int availableForWrite() override {
        // Estimate how many input bytes we can accept based on remaining buffer space
        // and our expansion factor.
        float expansion_factor = (44100.0f / (float)_info.sample_rate) * (2.0f / (float)_info.channels);
        int out_avail_bytes = _pcmBuffer.availableForWrite();
        return (int)((float)out_avail_bytes / expansion_factor);
    }

    bool begin() override {
        // Nothing to allocate dynamically.
        return true;
    }

private:
    PCMRingBuffer& _pcmBuffer;
    audio_tools::AudioInfo _info;
    float _phase_step;
    float _phase = 0.0f;
    int16_t _last_L = 0;
    int16_t _last_R = 0;
};
