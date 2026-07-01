#pragma once

class Command
{
public:
    void begin();
    void update();

private:
    void printHelp();
};

extern Command command;

// Legacy API (used by encoder.cpp)
void commandRotateCW();
void commandRotateCCW();

void commandClick();
void commandLongClick();