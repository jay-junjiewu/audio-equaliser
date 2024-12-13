#include "dsp.h"

void volumeGain_dB(AudioProcessor p, float volume_dB) {
    if (volume_dB < -48.0f || volume_dB > 48.0f) {
        std::cerr << "Error: Volume scale must be between -48dB and 48dB\n";
        return;
    }

    float gain = pow(10, volume_dB / 20.0);

    // Process left channel
    for (auto& sample : p.leftChannel) {
        int32_t scaledSample = static_cast<int32_t>(sample * gain);
        sample = std::max(std::min(scaledSample, INT16_MAX), INT16_MIN);
    }

    // Process right channel if stereo
    if (p.header.numChannels == 2) {
        for (auto& sample : p.rightChannel) {
            int32_t scaledSample = static_cast<int32_t>(sample * gain);
            sample = std::max(std::min(scaledSample, INT16_MAX), INT16_MIN);
        }
    }

    std::cout << "Total Gain of " << gain << " applied\n";

}