#include "dsp.h"

void volumeGain_dB(AudioProcessor& p, float gain_dB) {
    if (gain_dB < -48.0f || gain_dB > 48.0f) {
        std::cerr << "Error: Gain must be between -48dB and 48dB\n";
        return;
    }

    float gain = pow(10, gain_dB / 20.0);
    volumeGain(p, gain);
}

void volumeGain(AudioProcessor& p, float gain) {
    if (gain < 0.0f || gain > 255.0f) {
        std::cerr << "Error: Gain must be between 0 and 255\n";
        return;
    }

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

void filter(AudioProcessor& p, const std::vector<float>& b, const std::vector<float>& a) {
    if (b.empty() || a.empty()) {
        std::cerr << "Error: Filter coefficients must not be empty\n";
        return;
    }

    if (a[0] == 0.0f) {
        std::cerr << "Error: Filter denominator a0 should not be 0\n";
        return;
    }

    std::vector<float> b_norm = b;
    std::vector<float> a_norm = a;

    // Normalize to make a[0] == 1
    const float epsilon = 1e-6f;
    if (std::abs(a[0] - 1.0f) > epsilon) {
        float k = a[0];
        for (float &x : b_norm) x /= k;
        for (float &x : a_norm) x /= k;
    }

    p.leftChannel = applyFilter(p.leftChannel, b_norm, a_norm);

    if (p.header.numChannels == 2) {
        p.rightChannel = applyFilter(p.rightChannel, b_norm, a_norm);
    }
}

std::vector<int16_t> applyFilter(const std::vector<int16_t>& input, const std::vector<float>& b, const std::vector<float>& a) {
    std::vector<int16_t> filteredChannel(input.size(), 0);
    
    for (size_t i = 0; i < input.size(); i++) {
        int32_t sum = 0;  // int32_t to avoid overflow
        
        // b coefficients
        for (size_t j = 0; j < b.size(); j++) {
            if (i >= j) {
                sum += b[j] * static_cast<int32_t>(input[i - j]);  // b[k] * x[n-k]
            }
        }
        
        // a coefficients
        // Skip a[0] since it's normalized to 1
        for (size_t k = 1; k < a.size(); k++) {  
            if (i >= k) {
                sum -= a[k] * static_cast<int32_t>(filteredChannel[i - k]);  // a[k] * y[n-k]
            }
        }

        filteredChannel[i] = static_cast<int16_t>(std::min(std::max(sum, static_cast<int32_t>(INT16_MIN)), static_cast<int32_t>(INT16_MAX)));
    }
    
    return filteredChannel;
}

void filtfilt(AudioProcessor& p, const std::vector<float>& b, const std::vector<float>& a) {
    if (b.empty() || a.empty()) {
        std::cerr << "Error: Filter coefficients must not be empty\n";
        return;
    }

    if (a[0] == 0.0f) {
        std::cerr << "Error: Filter denominator a0 should not be 0\n";
        return;
    }

    std::vector<float> b_norm = b;
    std::vector<float> a_norm = a;

    // Normalize to make a[0] == 1
    const float epsilon = 1e-6f;
    if (std::abs(a[0] - 1.0f) > epsilon) {
        float k = a[0];
        for (float &x : b_norm) x /= k;
        for (float &x : a_norm) x /= k;
    }

    p.leftChannel = applyFiltfilt(p.leftChannel, b_norm, a_norm);

    if (p.header.numChannels == 2) {
        p.rightChannel = applyFiltfilt(p.rightChannel, b_norm, a_norm);
    }
}

std::vector<int16_t> applyFiltfilt(const std::vector<int16_t>& input, const std::vector<float>& b, const std::vector<float>& a) {
    std::vector<int16_t> forwardFiltered = applyFilter(input, b, a);

    std::reverse(forwardFiltered.begin(), forwardFiltered.end());

    std::vector<int16_t> reverseFiltered = applyFilter(forwardFiltered, b, a);

    std::reverse(reverseFiltered.begin(), reverseFiltered.end());

    return reverseFiltered;
}