#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "audio.h"

int main() {
    std::string inputFile = "audio/royalty_44.1k_16bit_stereo.wav";
    //std::string inputFile = "audio/royalty_44.1k_16bit_mono.wav";
    //std::string inputFile = "audio/royalty_16k_16bit_stereo.wav";
    //std::string inputFile = "audio/royalty_16k_16bit_mono.wav";

    //std::string inputFile = "audio/super_shy_44.1k_16bit_stereo.wav";
    //std::string inputFile = "audio/super_shy_16k_16bit_stereo.wav";

    std::string outputFile = "audio/output.wav";
    
    AudioProcessor p1(inputFile);

    p1.printWavHeader();
    //p1.writeOutputTxt("audio/rawBefore.txt");

    // Sub-Bass, Bass, Midrange, Upper Midrange, Treble
    int band = 2;   // 0 - 4
    filter(p1, p1.getB()[band], p1.getA()[band]);
    volumeGain(p1, 1);


    p1.writeOutputWav(outputFile);
    //p1.writeOutputTxt("audio/rawAfter.txt");

    return 0;
}
