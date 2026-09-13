#pragma once

#include <Arduino.h>

extern String outputLedState;

void controlsBegin();
void handleControlRequest(const String& request);
