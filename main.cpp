#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "file.h"

int main() {
    std::string inputFile = "audio/output.wav";   // Input WAV file
    std::string outputFile = "audio/output.wav";// Output WAV file
    

    printWavHeader(inputFile);

    reduceTotalVolume(inputFile, outputFile, 0.5);

    return 0;
}
