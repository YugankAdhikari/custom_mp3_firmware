#include <Arduino.h>

#include "encoder.h"
#include "pins.h"
#include "command.h"

Encoder encoder;

void Encoder::begin()
{
    pinMode(PIN_ENCODER_A, INPUT_PULLUP);
    pinMode(PIN_ENCODER_B, INPUT_PULLUP);

    lastState = digitalRead(PIN_ENCODER_A);
}

void Encoder::update()
{
    int currentState = digitalRead(PIN_ENCODER_A);

    if (currentState != lastState)
    {
        if (digitalRead(PIN_ENCODER_B) != currentState)
        {
            Serial.println(">>>>>>>>>> CLOCKWISE >>>>>>>>>>");
            commandRotateCW();
        }
        else
        {
            Serial.println("<<<<<<<<<< COUNTER CLOCKWISE <<<<<<<<<<");
            commandRotateCCW();
        }

        lastState = currentState;
    }
}
