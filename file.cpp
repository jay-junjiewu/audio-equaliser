#include "file.h"

void printWavHeader(std::string inputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
   
    if (!validWavFile(inputFile)) return;

    // Read WAV header
    WavHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    inFile.close();

    std::cout << "Chunk ID: " << std::string(header.chunkID, 4) << "\n";
    std::cout << "Chunk Size: " << header.chunkSize << " bytes\n";
    std::cout << "Format: " << std::string(header.format, 4) << "\n";
    std::cout << "Subchunk1 ID: " << std::string(header.subchunk1ID, 4) << "\n";
    std::cout << "Subchunk1 Size: " << header.subchunk1Size << " bytes\n";
    std::cout << "Audio Format: " << (header.audioFormat == 1 ? "PCM" : "Compressed") << "\n";
    std::cout << "Number of Channels: " << header.numChannels << "\n";
    std::cout << "Sample Rate: " << header.sampleRate << " Hz\n";
    std::cout << "Byte Rate: " << header.byteRate << " bytes/second\n";
    std::cout << "Block Align: " << header.blockAlign << " bytes\n";
    std::cout << "Bits Per Sample: " << header.bitsPerSample << " bits\n";
    std::cout << "Subchunk2 ID: " << std::string(header.subchunk2ID, 4) << "\n";
    std::cout << "Subchunk2 Size: " << header.subchunk2Size << " bytes\n";

    std::cout << '\n';
}


bool validWavFile(std::string inputFile) {
    // Open input file in binary mode
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Unable to open file " << inputFile << "\n";
        return false;;
    }

    // Read WAV header
    WavHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    inFile.close();

    if (std::string(header.chunkID, 4) != "RIFF" || std::string(header.format, 4) != "WAVE") {
        std::cerr << "Error: Not a valid WAV file.\n";
        return false;;
    }

    if (header.audioFormat != 1) {
        std::cerr << "Error: Only PCM format is supported.\n";
        return false;
    }

    if (header.bitsPerSample != 8 && header.bitsPerSample != 16) {
        std::cerr << "Error: Only 8-bit and 16-bit PCM files are supported.\n";
        return false;
    }

    if (header.numChannels == 1) {
        std::cout << "Mono audio detected.\n";
    } else if (header.numChannels == 2) {
        std::cout << "Stereo audio detected.\n";
    } else {
        std::cerr << "Error: Unsupported number of channels.\n";
        return false;
    }

    return true;
}

void reduceVolume(const std::string &inputFile, const std::string &outputFile, float volumeScale) {
    if (volumeScale < 0.0f || volumeScale > 1.0f) {
        std::cerr << "Error: Volume scale must be between 0 and 1.\n";
        return;
    }

    if (!validWavFile(inputFile)) return;

    std::ifstream inFile(inputFile, std::ios::binary);
    WavHeader header;
    inFile.read(reinterpret_cast<char *>(&header), sizeof(WavHeader));

    // Calculate actual data size
    inFile.seekg(0, std::ios::end);
    std::streampos fileSize = inFile.tellg();
    std::streampos dataStart = sizeof(WavHeader);
    size_t actualDataSize = fileSize - dataStart;

    // Read data
    inFile.seekg(dataStart, std::ios::beg);
    std::vector<uint8_t> rawData(actualDataSize);
    inFile.read(reinterpret_cast<char*>(rawData.data()), actualDataSize);
    inFile.close();

    if (header.bitsPerSample == 16) {
        auto *samples = reinterpret_cast<int16_t *>(rawData.data());
        size_t numSamples = rawData.size() / sizeof(int16_t);

        for (size_t i = 0; i < numSamples; i++) {
            // Clamp to prevent overflow
            int32_t scaledSample = static_cast<int32_t>(samples[i] * volumeScale);
            samples[i] = std::max(std::min(scaledSample, 32767), -32768);
        }
    } else if (header.bitsPerSample == 8) {
        for (size_t i = 0; i < rawData.size(); i++) {
            // Convert unsigned 8-bit to signed
            int adjustedSample = static_cast<int>(rawData[i]) - 128;
            
            int scaledSample = static_cast<int>(adjustedSample * volumeScale);
            
            // Convert back to unsigned and clamp
            int convertedSample = scaledSample + 128;
            convertedSample = std::max(0, std::min(convertedSample, 255));
            
            // Cast to uint8_t
            rawData[i] = static_cast<uint8_t>(convertedSample);
        }
    }

    // Open output file
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open output file.\n";
        return;
    }

    outFile.write("RIFF", 4);
    fileSize = 36 + rawData.size(); // 36 = header size
    outFile.write(reinterpret_cast<char*>(&fileSize), 4);
    outFile.write("WAVE", 4);
    outFile.write("fmt ", 4);
    uint32_t fmtChunkSize = 16; // Standard size for PCM
    outFile.write(reinterpret_cast<char*>(&fmtChunkSize), 4);
    
    // Write fmt chunk details
    uint16_t audioFormat = 1; // PCM
    outFile.write(reinterpret_cast<char*>(&audioFormat), 2);
    outFile.write(reinterpret_cast<char*>(&header.numChannels), 2);
    outFile.write(reinterpret_cast<char*>(&header.sampleRate), 4);
    outFile.write(reinterpret_cast<char*>(&header.byteRate), 4);
    outFile.write(reinterpret_cast<char*>(&header.blockAlign), 2);
    outFile.write(reinterpret_cast<char*>(&header.bitsPerSample), 2);

    // Write data chunk
    outFile.write("data", 4);
    uint32_t dataSize = rawData.size();
    outFile.write(reinterpret_cast<char*>(&dataSize), 4);

    // Write actual audio data
    outFile.write(reinterpret_cast<char*>(rawData.data()), rawData.size());

    outFile.close();

    std::cout << "Volume reduced and saved to " << outputFile << "\n";
}