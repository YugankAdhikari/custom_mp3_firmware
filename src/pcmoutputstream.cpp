#include "pcmoutputstream.h"

#include "ringbuffer.h"

PCMOutputStream pcmOutput;

bool PCMOutputStream::begin()
{
    active = true;
    return true;
}

void PCMOutputStream::end()
{
    active = false;
}

size_t PCMOutputStream::write(uint8_t byte)
{
    return write(&byte, 1);
}

size_t PCMOutputStream::write(const uint8_t *buffer, size_t size)
{
    if (!active)
        return 0;

    // RingBuffer already understands byte writes because it inherits Print.
    return pcmBuffer.write(buffer, size);
}