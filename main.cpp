#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "file.h"

int main() {
    std::string inputFile = "audio/royalty.wav";   // Input WAV file
    std::string outputFile = "audio/output.wav";// Output WAV file
    float volumeScale = 0.5f;

    printWavHeader(inputFile);

    reduceVolume(inputFile, outputFile, volumeScale);

    return 0;
}
