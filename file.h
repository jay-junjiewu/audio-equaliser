#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>


#pragma pack(push, 1)
// WAV file header structure
struct WavHeader {
    char chunkID[4];       // "RIFF"
    uint32_t chunkSize;    // File size - 8 bytes
    char format[4];        // "WAVE"
    char subchunk1ID[4];   // "fmt "
    uint32_t subchunk1Size;// Size of the fmt chunk
    uint16_t audioFormat;  // Audio format (1 for PCM)
    uint16_t numChannels;  // Number of channels
    uint32_t sampleRate;   // Sampling frequency
    uint32_t byteRate;     // Bytes per second
    uint16_t blockAlign;   // Bytes per sample (all channels)
    uint16_t bitsPerSample;// Bits per sample
    char subchunk2ID[4];   // "data"
    uint32_t subchunk2Size;// Number of bytes in data
};
#pragma pack(pop)


/// @brief Check if a .wav file is valid
/// @param inputFile 
/// @return bool if it is valid
bool validWavFile(std::string inputFile);

/// @brief Print WavHeader information
/// @param inputFile 
void printWavHeader(std::string inputFile);

/// @brief Reduces total volumn of the whole file
/// @param inputFile 
/// @param outputFile 
/// @param volumeScale 0.0f - 1.0f scale
void reduceTotalVolume(const std::string &inputFile, const std::string &outputFile, float volumeScale);

/// @brief Writes the data vector into a WAV file using the given header
/// @param outputFile 
/// @param header 
/// @param processedData 
void writeOutputWav(const std::string &outputFile, WavHeader &header, std::vector<uint8_t> &processedData);



#endif