#pragma once
#include "AudioDevices.h"

class AudioConfiguration
{
public:
    AudioConfiguration(AudioDevice primaryAudioOutput);

    void Apply();
    void Print() const;

    AudioDevice PrimaryAudioOutput;
};
