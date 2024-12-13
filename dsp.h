#ifndef DSP_H
#define DSP_H

#include "audio.h"



/// @brief Reduces total volumne of the whole file
/// @param volume_dB +-48.0f scale dB
void volumeGain_dB(AudioProcessor p, float volume_dB);



#endif