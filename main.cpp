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

    std::string outputFile = "audio/output.wav";
    
    AudioProcessor p1(inputFile);

    p1.printWavHeader();

    p1.reduceTotalVolume(0.5);

    p1.writeOutputWav(outputFile);

    // const std::vector<int16_t>& l = p1.getLeftChannel();
    // const std::vector<int16_t>& r = p1.getRightChannel();
    
    // std::cout << l.size() << " " << r.size() << '\n';

    // for (int i = 0; i < l.size(); i++) {
    //     std::cout << l[i] << " " << r[i] << '\n';
    // }

    return 0;
}
