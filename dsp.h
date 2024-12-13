#ifndef DSP_H
#define DSP_H

#include <algorithm>

#include "audio.h"


/// @brief Reduces total volumne of the whole file
/// @param gain_dB -48.0f - 48.9f scale dB
void volumeGain_dB(AudioProcessor& p, float gain_dB);


/// @brief Reduces total volumne of the whole file
/// @param p Reference to AudioProcessor object
/// @param gain 0 - 255 scale
void volumeGain(AudioProcessor& p, float gain);


/// @brief Filters all channels of AudioProcessor object
/// @param p Reference to AudioProcessor object
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
void filter(AudioProcessor& p, const std::vector<float>& b, const std::vector<float>& a);


/// @brief Filters data based on the filter coefficients
/// @param input data to filter
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
/// @return vector of filtered data 
std::vector<int16_t> applyFilter(const std::vector<int16_t>& input, const std::vector<float>& b, const std::vector<float>& a);


/// @brief Zero-phase filtering of all channels of AudioProcessor object
/// @param p Reference to AudioProcessor object
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
void filtfilt(AudioProcessor& p, const std::vector<float>& b, const std::vector<float>& a);


/// @brief Zero-phase filtering based on the filter coefficients
/// @param input data to filter
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
/// @return vector of filtered data 
std::vector<int16_t> applyFiltfilt(const std::vector<int16_t>& input, const std::vector<float>& b, const std::vector<float>& a);


#endif