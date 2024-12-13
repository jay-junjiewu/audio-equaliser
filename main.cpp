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

    volumeGain_dB(p1, 3);

    p1.writeOutputWav(outputFile);

    const std::vector<int16_t>& l = p1.getLeftChannel();
    const std::vector<int16_t>& r = p1.getRightChannel();
    //const std::vector<char>& ld = p1.getListData();
    
    // std::cout << l.size() << " " << r.size() << '\n';

    std::string rawDump = "rawDump.txt";
    std::ofstream rawFile;
    rawFile.open(rawDump);
    if (!rawFile) {
        throw std::runtime_error("Unable to open file: " + rawDump);
    }


    for (int i = 0; i < l.size(); i++) {
        rawFile << l[i] << " " << r[i] << '\n';
        //rawFile << l[i] << '\n';
    }

    rawFile.close();

    

    return 0;
}
