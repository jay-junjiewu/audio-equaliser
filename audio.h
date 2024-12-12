#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>


class AudioProcessor {
public:

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



    // Constructors
    AudioProcessor() = default;
    AudioProcessor(const std::string& inputFile);

    // Getters for channel data
    const std::vector<int16_t>& getLeftChannel() const { return leftChannel; }
    const std::vector<int16_t>& getRightChannel() const { return rightChannel; }

    // Get header
    const WavHeader& getHeader() const { return header; }


    /// @brief Check if a .wav file is valid
    /// @return bool
    bool validWavFile();

    /// @brief Print WavHeader information
    void printWavHeader();

    /// @brief Reduces total volumne of the whole file
    /// @param volumeScale 0.0f - 1.0f scale
    void reduceTotalVolume(float volumeScale);

    /// @brief Writes the data vector into a WAV file using the given header
    /// @param outputFile 
    /// @param header 
    /// @param processedData 
    void writeOutputWav(const std::string& outputFile);



private:
    WavHeader header;                   // WAV header
    std::vector<int16_t> leftChannel;   // Left channel audio samples
    std::vector<int16_t> rightChannel;  // Right channel audio samples


};

#endif