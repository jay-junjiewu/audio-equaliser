#ifndef AUDIO_H
#define AUDIO_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <climits>


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
        char subchunk2ID[4];   // "LIST"
        uint32_t subchunk2Size;// Number of bytes in LIST
    };
    #pragma pack(pop)

    // Constructors
    AudioProcessor() = default;
    AudioProcessor(const std::string& inputFile);

    // Getters for private data
    const std::vector<int16_t>& getLeftChannel() const { return leftChannel; }
    const std::vector<int16_t>& getRightChannel() const { return rightChannel; }
    const std::vector<char>& getListData() const { return listData; }
    const WavHeader& getHeader() const { return header; }

    /// @brief Check if a .wav file is valid
    /// @return bool
    bool validWavFile();

    /// @brief Print WavHeader information
    void printWavHeader();

    /// @brief Writes the left and right channel vector into a WAV file
    /// @param outputFile 
    void writeOutputWav(const std::string& outputFile);


    /// @brief Reduces total volumne of the whole file
    /// @param volume_dB +-48.0f scale dB
    friend void volumeGain_dB(AudioProcessor p, float volume_dB);



private:
    WavHeader header;                   // WAV header
    std::vector<int16_t> leftChannel;   // Left channel audio samples
    std::vector<int16_t> rightChannel;  // Right channel audio samples
    std::vector<char> listData;         // LIST data

};

#endif