#ifndef DSP_H
#define DSP_H

#include <algorithm>

#include "audio.h"


/// @brief Reduces total volumne of the whole file
/// @param gain_dB -48.0f - 48.9f scale dB
/// @param sel Channel selection: left 'L', right 'R' or both 'B'
/// @param startDuration in seconds
/// @param endDuration in seconds
void volumeGain_dB(AudioProcessor& p, float gain_dB, char sel, float startDuration, float endDuration);


/// @brief Reduces total volumne of the whole file
/// @param p Reference to AudioProcessor object
/// @param gain 0 - 255 scale
/// @param sel Channel selection: left 'L', right 'R' or both 'B'
/// @param startDuration in seconds
/// @param endDuration in seconds
void volumeGain(AudioProcessor& p, float gain, char sel, float startDuration, float endDuration);


/// @brief Scales input data based on gain
/// @param input data to scale
/// @param gain 0 - 255 scale
/// @return vector of scaled data 
/// @param startIndex inclusive
/// @param endIndex not inclusive
std::vector<int16_t> applyVolumeGain(const std::vector<int16_t>& input, float gain, int startIndex, int endIndex);


/// @brief Filters all channels of AudioProcessor object
/// @param p Reference to AudioProcessor object
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
/// @param sel Channel selection: left 'L', right 'R' or both 'B'
void filter(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a, char sel);


/// @brief Filters data based on the filter coefficients
/// @param input data to filter
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
/// @return vector of filtered data 
std::vector<int16_t> applyFilter(const std::vector<int16_t>& input, const std::vector<double>& b, const std::vector<double>& a);


/// @brief Zero-phase filtering of all channels of AudioProcessor object
/// @param p Reference to AudioProcessor object
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
/// @param sel Channel selection: left 'L', right 'R' or both 'B'
void filtfilt(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a, char sel);


/// @brief Zero-phase filtering based on the filter coefficients
/// @param input data to filter
/// @param b Numerator Coefficents {b0, b1, b2, ...}
/// @param a Denominator Coefficents {1, a1, a2, a3, ...}
/// @return vector of filtered data 
std::vector<int16_t> applyFiltfilt(const std::vector<int16_t>& input, const std::vector<double>& b, const std::vector<double>& a);


/// @brief Applies 5 gains to the preset 5 equaliser filters
/// @param p Reference to AudioProcessor object
/// @param gains 5 gains for Sub-Bass, Bass, Midrange, Upper Midrange, Treble
/// @param sel Channel selection: left 'L', right 'R' or both 'B'
void equaliser(AudioProcessor& p, const std::vector<float>& gains, char sel);


/// @brief Applies dynamic range compression to all audio channels
/// @param p Reference to AudioProcessor object
/// @param threshold 0.0f - 1.0f, level above which to apply gain reduction
/// @param ratio >= 1, degree of compression
/// @param makeUpGain 1.0f - 3.0f, whole signal gain to bring output level back up
/// @param startDuration in seconds
/// @param endDuration in seconds
void dynamicCompression(AudioProcessor& p, float threshold, int ratio, float makeUpGain, float startDuration, float endDuration);

/// @brief Reverses the entire audio
/// @param p Reference to AudioProcessor object
void reverseAudio(AudioProcessor& p);


#endif