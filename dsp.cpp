#include "dsp.h"

void volumeGain_dB(AudioProcessor& p, float gain_dB, char sel, float startDuration, float endDuration) {
    if (gain_dB < -48.0f || gain_dB > 48.0f) {
        std::cerr << "Error: Gain must be between -48dB and 48dB\n\n";
        return;
    }

    float gain = pow(10, gain_dB / 20.0);
    volumeGain(p, gain, sel, startDuration, endDuration);
}

void volumeGain(AudioProcessor& p, float gain, char sel, float startDuration, float endDuration) {
    if (gain < 0.0f || gain > 255.0f) {
        std::cerr << "Error: Gain must be between 0 and 255\n\n";
        return;
    }

    sel = tolower(sel);
    if (sel != 'l' && sel != 'r' && sel != 'b') {
        std::cerr << "Error: Invalid channel selection (l, r, or b) \n\n";
        return;
    }

    if (sel == 'r' && p.getRightChannel().empty()) {
        std::cerr << "Audio is mono and does not have a right channel" << "\n\n";
        return;
    }
    
    if (sel == 'b' && p.getRightChannel().empty()) {
        sel = 'l';
    }

    if (startDuration < 0.0f || startDuration > p.totalDuration) {
        std::cerr << "Error: Start duration must be between 0 and " << p.totalDuration << " sec\n\n";
        return;
    }

    if (endDuration < 0.0f || endDuration > p.totalDuration) {
        std::cerr << "Error: End duration must be between 0 and " << p.totalDuration << " sec\n\n";
        return;
    }

    if (startDuration > endDuration) {
        std::cerr << "Error: Start duration must be before end duration\n\n";
        return;
    }  

    int startIndex = startDuration * p.header.sampleRate;
    int endIndex = endDuration * p.header.sampleRate;

    // Process left channel
    if (sel == 'l' || sel == 'b') {
        p.leftChannel = applyVolumeGain(p.leftChannel, gain, startIndex, endIndex);
    }

    // Process right channel
    if (sel == 'r' || sel == 'b') {
        p.rightChannel = applyVolumeGain(p.rightChannel, gain, startIndex, endIndex);
    }

    std::cout << "Successfully applied gain of " << gain << " to ";
    if (sel == 'l')
        std::cout << "left channel ";
    else if (sel == 'r')
        std::cout << "right channel ";
    else if (sel == 'b')
        std::cout << "left and right channels ";

    std::cout << "from " << startDuration << " to " << endDuration << " sec ";
    std::cout << "[" << startIndex << " - " << endIndex << ")\n\n";
}

std::vector<int16_t> applyVolumeGain(const std::vector<int16_t>& input, float gain, int startIndex, int endIndex) {
    if (gain < 0.0f || gain > 255.0f) {
        std::cerr << "Error: Gain must be between 0 and 255\n\n";
        return {};
    }

    if (startIndex < 0 || startIndex > input.size()) {
        std::cerr << "Error: Start index must be between 0 and " << input.size() << " sec\n\n";
        return {};
    }

    if (endIndex < 0 || endIndex > input.size()) {
        std::cerr << "Error: End index must be between 0 and " << input.size() << " sec\n\n";
        return {};
    }

    if (startIndex > endIndex) {
        std::cerr << "Error: Start index must be before end index\n\n";
        return {};
    }  

    std::vector<int16_t> gainChannel = input;

    for (size_t i = startIndex; i < endIndex; i++) {
        int32_t scaledSample = static_cast<int32_t>(input[i]) * gain;
        gainChannel[i] = std::max(std::min(scaledSample, INT16_MAX), INT16_MIN);
    }

    return gainChannel;
}

void filter(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a, char sel) {
    if (b.empty() || a.empty()) {
        std::cerr << "Error: Filter coefficients must not be empty\n\n";
        return;
    }

    if (a[0] == 0.0f) {
        std::cerr << "Error: Filter denominator a0 should not be 0\n\n";
        return;
    }

    sel = tolower(sel);
    if (sel != 'l' && sel != 'r' && sel != 'b') {
        std::cerr << "Error: Invalid channel selection (l, r, or b) \n\n";
        return;
    }

    if (sel == 'r' && p.getRightChannel().empty()) {
        std::cerr << "Audio is mono and does not have a right channel" << "\n\n";
        return;
    }
    
    if (sel == 'b' && p.getRightChannel().empty()) {
        sel = 'l';
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

    // Process left channel
    if (sel == 'l' || sel == 'b') {
        p.leftChannel = applyFilter(p.leftChannel, b_norm, a_norm);
    }

    // Process right channel
    if (sel == 'r' || sel == 'b') {
        p.rightChannel = applyFilter(p.rightChannel, b_norm, a_norm);
    }

    std::cout << "Successfully applied filter on ";
    if (sel == 'l')
        std::cout << "left channel\n\n";
    else if (sel == 'r')
        std::cout << "right channel\n\n";
    else if (sel == 'b')
        std::cout << "left and right channels\n\n";

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

void filtfilt(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a, char sel) {
    if (b.empty() || a.empty()) {
        std::cerr << "Error: Filter coefficients must not be empty\n\n";
        return;
    }

    if (a[0] == 0.0f) {
        std::cerr << "Error: Filter denominator a0 should not be 0\n\n";
        return;
    }

    sel = tolower(sel);
    if (sel != 'l' && sel != 'r' && sel != 'b') {
        std::cerr << "Error: Invalid channel selection (l, r, or b) \n\n";
        return;
    }

    if (sel == 'r' && p.getRightChannel().empty()) {
        std::cerr << "Audio is mono and does not have a right channel" << "\n\n";
        return;
    }
    
    if (sel == 'b' && p.getRightChannel().empty()) {
        sel = 'l';
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

    // Process left channel
    if (sel == 'l' || sel == 'b') {
        p.leftChannel = applyFiltfilt(p.leftChannel, b_norm, a_norm);
    }

    // Process right channel
    if (sel == 'r' || sel == 'b') {
        p.rightChannel = applyFiltfilt(p.rightChannel, b_norm, a_norm);
    }

    std::cout << "Successfully applied filtfilt on ";
    if (sel == 'l')
        std::cout << "left channel\n\n";
    else if (sel == 'r')
        std::cout << "right channel\n\n";
    else if (sel == 'b')
        std::cout << "left and right channels\n\n";
}

std::vector<int16_t> applyFiltfilt(const std::vector<int16_t>& input, const std::vector<double>& b, const std::vector<double>& a) {
    std::vector<int16_t> forwardFiltered = applyFilter(input, b, a);

    std::reverse(forwardFiltered.begin(), forwardFiltered.end());

    std::vector<int16_t> reverseFiltered = applyFilter(forwardFiltered, b, a);

    std::reverse(reverseFiltered.begin(), reverseFiltered.end());

    return reverseFiltered;
}

void equaliser(AudioProcessor& p, const std::vector<float>& gains, char sel) {
    if (gains.size() != 5) {
        std::cerr << "Error: Equaliser needs 5 gains\n\n";
        return;
    }

    for (float g : gains) {
        if (g < 0.0f || g > 255.0f) {
            std::cerr << "Error: Gain must be between 0 and 255\n\n";
            return;
        }
    }

    sel = tolower(sel);
    if (sel != 'l' && sel != 'r' && sel != 'b') {
        std::cerr << "Error: Invalid channel selection (l, r, or b) \n\n";
        return;
    }

    if (sel == 'r' && p.getRightChannel().empty()) {
        std::cerr << "Audio is mono and does not have a right channel" << "\n\n";
        return;
    }

    if (sel == 'b' && p.getRightChannel().empty()) {
        sel = 'l';
    }

    if (sel == 'l' || sel == 'b') {
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
    }

    if (sel == 'r' || sel == 'b') {
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

    std::cout << "Equalised ";
    if (sel == 'l')
        std::cout << "left channel ";
    else if (sel == 'r')
        std::cout << "right channel ";
    else if (sel == 'b')
        std::cout << "left and right channels ";
    
    std::cout << "with gains: ";
    for (float g : gains) {
        std::cout << g << " ";
    }
    std::cout << "\n\n";
}

void dynamicCompression(AudioProcessor& p, float threshold, int ratio, float makeUpGain, float startDuration, float endDuration) {
    if (threshold < 0.0f || threshold > 1.0f) {
        std::cerr << "Error: Threshold must be between 0.0 and 1.0\n\n";
        return;
    }

    if (ratio <= 0) {
        std::cerr << "Error: Ratio must be greater than 1\n\n";
        return;
    }

    if (makeUpGain < 1.0f || makeUpGain > 3.0f) {
        std::cerr << "Error: Make-up gain must be between 1.0 and 3.0\n\n";
        return;
    }

    if (startDuration < 0.0f || startDuration > p.totalDuration) {
        std::cerr << "Error: Start duration must be between 0 and " << p.totalDuration << " sec\n\n";
        return;
    }

    if (endDuration < 0.0f || endDuration > p.totalDuration) {
        std::cerr << "Error: End duration must be between 0 and " << p.totalDuration << " sec\n\n";
        return;
    }

    if (startDuration > endDuration) {
        std::cerr << "Error: Start duration must be before end duration\n\n";
        return;
    }  

    int startIndex = startDuration * p.header.sampleRate;
    int endIndex = endDuration * p.header.sampleRate;

    int thresholdInt = threshold * INT16_MAX;
    
    for (size_t j = startIndex; j < endIndex; j++) {
        int32_t compressedL = static_cast<int32_t>(p.leftChannel[j]);
        
        if (abs(compressedL) > thresholdInt) {
            if (compressedL > 0)
                compressedL = thresholdInt + (compressedL - thresholdInt) / ratio;
            else
                compressedL = -thresholdInt + (compressedL + thresholdInt) / ratio;
        }
        compressedL *= makeUpGain;

        compressedL = std::clamp(compressedL, static_cast<int32_t>(INT16_MIN), static_cast<int32_t>(INT16_MAX));
        p.leftChannel[j] = static_cast<int16_t>(compressedL);
    }

    if (p.getHeader().numChannels == 2) {
        for (size_t j = startIndex; j < endIndex; j++) {
            int32_t compressedR = static_cast<int32_t>(p.rightChannel[j]);

            if (abs(compressedR) > thresholdInt) {
                if (compressedR > 0)
                    compressedR = thresholdInt + (compressedR - thresholdInt) / ratio;
                else
                    compressedR = -thresholdInt + (compressedR + thresholdInt) / ratio;
            }
            compressedR *= makeUpGain;

            compressedR = std::clamp(compressedR, static_cast<int32_t>(INT16_MIN), static_cast<int32_t>(INT16_MAX));
            p.rightChannel[j] = static_cast<int16_t>(compressedR);
        }
    }
  
    std::cout << "Dynamically compressed audio with threshold " << threshold;
    std::cout << ", ratio " << ratio << ":1,";
    std::cout << " and make-up gain " << makeUpGain;
    std::cout << " from " << startDuration << " to " << endDuration << " sec ";
    std::cout << "[" << startIndex << " - " << endIndex << ")\n\n";
}

void reverseAudio(AudioProcessor& p) {
    std::reverse(p.leftChannel.begin(), p.leftChannel.end());

    if (p.getHeader().numChannels == 2) 
        std::reverse(p.rightChannel.begin(), p.rightChannel.end());

    std::cout << "Successfully reversed audio \n\n";
}