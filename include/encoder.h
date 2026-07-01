#pragma once

class Encoder
{
public:
    void begin();
    void update();

private:
    int lastState = HIGH;
};

extern Encoder encoder;