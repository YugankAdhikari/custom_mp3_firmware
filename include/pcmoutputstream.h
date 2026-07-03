#pragma once

#include <Arduino.h>
#include <Print.h>

class PCMOutputStream : public Print
{
public:
    bool begin();

    void end();

    size_t write(uint8_t byte) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    using Print::write;

private:
    bool active = false;
};

extern PCMOutputStream pcmOutput;