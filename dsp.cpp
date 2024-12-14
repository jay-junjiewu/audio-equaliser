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
    p.leftChannel = applyVolumeGain(p.leftChannel, gain);

    // Process right channel if stereo
    if (p.header.numChannels == 2) {
        p.rightChannel = applyVolumeGain(p.rightChannel, gain);
    }

    std::cout << "Successfully applied gain of " << gain << "\n\n";
}

std::vector<int16_t> applyVolumeGain(const std::vector<int16_t>& input, float gain) {
    if (gain < 0.0f || gain > 255.0f) {
        std::cerr << "Error: Gain must be between 0 and 255\n";
        return {};
    }

    std::vector<int16_t> gainChannel(input.size(), 0);

    for (size_t i = 0; i < input.size(); i++) {
        int32_t scaledSample = static_cast<int32_t>(input[i]) * gain;
        gainChannel[i] = std::max(std::min(scaledSample, INT16_MAX), INT16_MIN);
    }

    return gainChannel;
}

void filter(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a) {
    if (b.empty() || a.empty()) {
        std::cerr << "Error: Filter coefficients must not be empty\n";
        return;
    }

    if (a[0] == 0.0f) {
        std::cerr << "Error: Filter denominator a0 should not be 0\n";
        return;
    }

    std::vector<double> b_norm = b;
    std::vector<double> a_norm = a;

    // Normalize to make a[0] == 1
    const float epsilon = 1e-6f;
    if (std::abs(a[0] - 1.0f) > epsilon) {
        float k = a[0];
        for (double &x : b_norm) x /= k;
        for (double &x : a_norm) x /= k;
        std::cout << "Normalised Filter Coefficients\n";
    }

    p.leftChannel = applyFilter(p.leftChannel, b_norm, a_norm);

    if (p.header.numChannels == 2) {
        p.rightChannel = applyFilter(p.rightChannel, b_norm, a_norm);
    }
}

std::vector<int16_t> applyFilter(const std::vector<int16_t>& input, const std::vector<double>& b, const std::vector<double>& a) {
    /*

        b0 + b1*z^(-1) + b2*z^(-2) + ...
    H = --------------------------------
        1 + a1*z^(-1) + a2*z^(-2) + ...


    y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]    
    
    */

    std::vector<int16_t> filteredChannel(input.size(), 0);
    
    for (size_t i = 0; i < input.size(); i++) {
        double sum = 0.0;  // to avoid overflow and maintian precision
        
        // b coefficients
        for (size_t j = 0; j < b.size(); j++) {
            if (i >= j) {
                // b[j] * x[n-j]
                sum += b[j] * static_cast<double>(input[i - j]);  
            }
        }
        
        // a coefficients
        // Skip a[0] since it's normalized to 1
        for (size_t k = 1; k < a.size(); k++) {  
            if (i >= k) {
                // a[k] * y[n-k]
                sum -= a[k] * static_cast<double>(filteredChannel[i - k]);    
            }
        }

        filteredChannel[i] = static_cast<int16_t>(std::min(std::max(sum, static_cast<double>(INT16_MIN)), static_cast<double>(INT16_MAX)));
    }
    
    return filteredChannel;
}

void filtfilt(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a) {
    if (b.empty() || a.empty()) {
        std::cerr << "Error: Filter coefficients must not be empty\n";
        return;
    }

    if (a[0] == 0.0f) {
        std::cerr << "Error: Filter denominator a0 should not be 0\n";
        return;
    }

    std::vector<double> b_norm = b;
    std::vector<double> a_norm = a;

    // Normalize to make a[0] == 1
    const float epsilon = 1e-6f;
    if (std::abs(a[0] - 1.0f) > epsilon) {
        float k = a[0];
        for (double &x : b_norm) x /= k;
        for (double &x : a_norm) x /= k;
    }

    p.leftChannel = applyFiltfilt(p.leftChannel, b_norm, a_norm);

    if (p.header.numChannels == 2) {
        p.rightChannel = applyFiltfilt(p.rightChannel, b_norm, a_norm);
    }
}

std::vector<int16_t> applyFiltfilt(const std::vector<int16_t>& input, const std::vector<double>& b, const std::vector<double>& a) {
    std::vector<int16_t> forwardFiltered = applyFilter(input, b, a);

    std::reverse(forwardFiltered.begin(), forwardFiltered.end());

    std::vector<int16_t> reverseFiltered = applyFilter(forwardFiltered, b, a);

    std::reverse(reverseFiltered.begin(), reverseFiltered.end());

    return reverseFiltered;
}

void equaliser(AudioProcessor& p, const std::vector<float>& gains) {
    if (gains.size() != 5) {
        std::cerr << "Error: Equaliser needs 5 gains\n";
        return;
    }

    for (float g : gains) {
        if (g < 0.0f || g > 255.0f) {
            std::cerr << "Error: Gain must be between 0 and 255\n";
            return;
        }
    }

    // Cache leftChannel reference
    auto& leftChannel = p.leftChannel;

    std::vector<int16_t> accumulatedL(leftChannel.size(), 0);

    for (size_t i = 0; i < gains.size(); i++) {
        std::vector<int16_t> filteredL = applyFiltfilt(leftChannel, p.getB()[i], p.getA()[i]);

        for (size_t j = 0; j < leftChannel.size(); j++) {
            // 0.7 cause filter overlap causes higher gain when all 5 signals are added up
            int32_t scaledSampleL = static_cast<int32_t>(filteredL[j]) * 0.7 * gains[i];
            accumulatedL[j] += std::clamp(scaledSampleL, static_cast<int32_t>(INT16_MIN), static_cast<int32_t>(INT16_MAX));
        }
    }
    leftChannel = std::move(accumulatedL);
    

    if (p.header.numChannels == 2) {
        auto& rightChannel = p.rightChannel;

        std::vector<int16_t> accumulatedR(rightChannel.size(), 0);

        for (size_t i = 0; i < gains.size(); i++) {
            std::vector<int16_t> filteredR = applyFiltfilt(rightChannel, p.getB()[i], p.getA()[i]);

            for (size_t j = 0; j < rightChannel.size(); j++) {
                int32_t scaledSampleR = static_cast<int32_t>(filteredR[j]) * 0.7 * gains[i];
                accumulatedR[j] += std::clamp(scaledSampleR, static_cast<int32_t>(INT16_MIN), static_cast<int32_t>(INT16_MAX));
            }
        }
        rightChannel = std::move(accumulatedR);
    }

    std::cout << "Equalised with gains: ";
    for (float g : gains) {
        std::cout << g << " ";
    }
    std::cout << "\n\n";
}