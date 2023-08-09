#include "AudioConfiguration.h"
#include <iostream>
#include <DefSound/PolicyConfig.h>
#include <comdef.h>

AudioConfiguration::AudioConfiguration(AudioDevice primaryAudioOutput)
    : PrimaryAudioOutput(primaryAudioOutput) { }

void AudioConfiguration::Print() const
{
    std::wcout << "Primary audio output device: " << std::endl
        << '\t' << PrimaryAudioOutput.GetDisplayName()
        << '(' << PrimaryAudioOutput.GetId() << ')' << std::endl;
}

void AudioConfiguration::Apply()
{
    IPolicyConfigVista* policyConfig;

    HRESULT status = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL,
                                      __uuidof(IPolicyConfigVista), (LPVOID*) &policyConfig);
    
    status = policyConfig->SetDefaultEndpoint(PrimaryAudioOutput.GetId().c_str(), eConsole);
    policyConfig->Release();

    if (SUCCEEDED(status))
    {
        std::wcout << "[INFO] Switching primary audio output device to "
            << PrimaryAudioOutput.GetDisplayName() << std::endl;
    }
    else
    {
        _com_error err(status);
        std::cout << err.ErrorMessage() << std::endl;
        throw std::exception("Failed to switch primary audio output device");
    }
}