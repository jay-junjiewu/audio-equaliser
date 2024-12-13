#ifndef DSP_H
#define DSP_H


#include "audio.h"



/// @brief Reduces total volumne of the whole file
/// @param gain_dB -48.0f - 48.9f scale dB
void volumeGain_dB(AudioProcessor& p, float gain_dB);

/// @brief Reduces total volumne of the whole file
/// @param p Reference to AudioProcessor object
/// @param gain 0 - 255 scale
void volumeGain(AudioProcessor& p, float gain);

/// @brief Filters the audio based in filter coefficients
/// @param p Reference to AudioProcessor object
/// @param b Numerator Coefficents
/// @param a Denominator Coefficents
void filter(AudioProcessor& p, std::vector<int> b, std::vector<int> a);


#endif