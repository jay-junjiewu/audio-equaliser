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
    // Constructor Helper
    void initialise(const std::string& inputFile);

    // Getters for private data
    const WavHeader& getHeader() const { return header; }
    const float& getDuration() const { return totalDuration; }
    const std::vector<int16_t>& getLeftChannel() const { return leftChannel; }
    const std::vector<int16_t>& getRightChannel() const { return rightChannel; }
    const std::vector<char>& getListData() const { return listData; }
    const std::vector<std::vector<double>>& getB() const { return b; }
    const std::vector<std::vector<double>>& getA() const { return a; }

    /// @brief Check if a .wav file is valid
    /// @return bool
    bool validWavFile();

    /// @brief Print WavHeader information
    void printWavHeader();

    /// @brief Writes the left and right channel vector into a WAV file
    /// @param outputFile 
    void writeOutputWav(const std::string& outputFile);

    // @brief Writes the left and right channel vector into a txt file
    /// @param outputFile 
    void writeOutputTxt(const std::string& outputFile);

    /// @brief 
    /// @param startDuration 
    /// @param endDuration 
    void trimAudio(float startDuration, float endDuration);


    /*************************** Friendly DSP Functions *****************************/ 

    friend void volumeGain_dB(AudioProcessor& p, float gain_dB, char sel);

    friend void volumeGain(AudioProcessor& p, float gain, char sel);

    friend void filter(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a, char sel);

    friend void filtfilt(AudioProcessor& p, const std::vector<double>& b, const std::vector<double>& a, char sel);

    friend void equaliser(AudioProcessor& p, const std::vector<float>& gains, char sel);

    friend void dynamicCompression(AudioProcessor& p, float threshold, int ratio, float makeUpGain);



private:
    WavHeader header;                   // WAV header

    float totalDuration;                // Duration of the audio file

    std::vector<int16_t> leftChannel;   // Left channel audio samples
    std::vector<int16_t> rightChannel;  // Right channel audio samples

    std::vector<char> listData;         // LIST data

    // Equaliser Filter Coefficients
    std::vector<std::vector<double>> b;
    std::vector<std::vector<double>> a;

};

#endif