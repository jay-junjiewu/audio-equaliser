#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "audio.h"

int main() {
    //std::string inputFile = "audio/royalty_44.1k_16bit_stereo.wav";
    //std::string inputFile = "audio/royalty_44.1k_16bit_mono.wav";
    //std::string inputFile = "audio/royalty_16k_16bit_stereo.wav";
    //std::string inputFile = "audio/royalty_16k_16bit_mono.wav";

    std::string inputFile = "audio/super_shy_44.1k_16bit_stereo.wav";
    //std::string inputFile = "audio/super_shy_16k_16bit_stereo.wav";

    std::string outputFile = "audio/output.wav";
    
    AudioProcessor p1(inputFile);

    p1.printWavHeader();
    //p1.writeOutputTxt("audio/rawBefore.txt");

    // volumeGain(p1, 1);

    //equaliser(p1, {1.0, 1.0, 1.0, 1.0, 1.0});

    equaliser(p1, {0.0, 0.0, 1.0, 0.0, 0.0});


    // std::vector<double> b = {
    //     8.0547e-05,0,-0.00016109,0,8.0547e-05
    // };
    // std::vector<double> a = {
    //    1,-3.9741,5.9229,-3.9236,0.97478
    // };

    // int band = 0;   // 0 - 4
    // filter(p1, p1.getB()[band], p1.getA()[band]);


    p1.writeOutputWav(outputFile);
    //p1.writeOutputTxt("audio/rawAfter.txt");

    return 0;
}
