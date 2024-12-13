#include "audio.h"

AudioProcessor::AudioProcessor(const std::string& inputFile) {
    // Open input file in binary mode
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Unable to open file: " + inputFile);
    }

    // Read WAV header
    inFile.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

    if (!validWavFile()) {
        throw std::runtime_error("Invalid WAV file: " + inputFile);
    }    

    // Read LIST chunk ID and size
    if (strncmp(header.subchunk2ID, "LIST", 4) == 0 && header.subchunk2Size > 0) {
        listData.resize(header.subchunk2Size);
        if (!inFile.read(listData.data(), header.subchunk2Size)) {
            throw std::runtime_error("Failed to read LIST chunk");
        }
    }

    // Read data chunk ID and size
    char chunkID[5] = {0};  // +1 for null terminator
    u_int32_t dataSize;
    if (!inFile.read(chunkID, 4) || 
        !inFile.read(reinterpret_cast<char*>(&dataSize), 4) || 
        strcmp(chunkID, "data") != 0) {
        throw std::runtime_error("Failed to read data chunk");
    }

    // Read data    
    std::vector<uint8_t> rawData(dataSize);
    inFile.read(reinterpret_cast<char*>(rawData.data()), dataSize);
    inFile.close();


    // Separate channels
    leftChannel.clear();
    rightChannel.clear();

    auto *samples = reinterpret_cast<int16_t *>(rawData.data());
    size_t numSamples = rawData.size() / sizeof(int16_t);
    size_t numChannelSamples = numSamples / header.numChannels;
    
    for (size_t i = 0; i < numChannelSamples; i++) {
        leftChannel.push_back(samples[i * header.numChannels]);
        if (header.numChannels == 2) {
            rightChannel.push_back(samples[i * header.numChannels + 1]);
        }
    }
}


void AudioProcessor::printWavHeader() {
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


bool AudioProcessor::validWavFile() {
    if (std::string(header.chunkID, 4) != "RIFF" || std::string(header.format, 4) != "WAVE") {
        std::cerr << "Error: Not a valid WAV file.\n";
        return false;;
    }

    if (header.audioFormat != 1) {
        std::cerr << "Error: Only PCM format is supported.\n";
        return false;
    }

    if ( header.bitsPerSample != 16) {
        std::cerr << "Error: Only 16-bit PCM files are supported.\n";
        return false;
    }

    if (header.numChannels != 1 && header.numChannels != 2) {
        std::cerr << "Error: Unsupported number of channels (Only Stereo and Mono).\n";
        return false;
    }

    return true;
}


void AudioProcessor::writeOutputWav(const std::string& outputFile) {
    if (leftChannel.empty() || (header.numChannels == 2 && rightChannel.empty())) {
        throw std::runtime_error("No audio data to write");
    }

    // Open output file
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Unable to open output file: " + outputFile);
    }

    std::vector<int16_t> interleavedData;
    
    // Ensure matching channel lengths
    size_t numSamples = std::min(leftChannel.size(), rightChannel.size());
    interleavedData.reserve(numSamples * 2);

    // Correctly interleave stereo channels
    if (header.numChannels == 2) {
        for (size_t i = 0; i < numSamples; ++i) {
            interleavedData.push_back(leftChannel[i]);
            interleavedData.push_back(rightChannel[i]);
        }
    } else {
        // Mono case
        interleavedData = leftChannel;
    }

    // Calculate sizes
    uint32_t dataSize = interleavedData.size() * sizeof(int16_t);
    uint32_t outputFileSize = sizeof(WavHeader) - 8 + dataSize;

    // Write RIFF chunk
    outFile.write(header.chunkID, 4);
    outFile.write(reinterpret_cast<char*>(&outputFileSize), 4);
    outFile.write(header.format, 4);

    // Write fmt chunk
    outFile.write("fmt ", 4);
    uint32_t fmtChunkSize = 16; // Standard size for PCM
    outFile.write(reinterpret_cast<char*>(&fmtChunkSize), 4);
    
    // Write fmt chunk details directly from header
    outFile.write(reinterpret_cast<char*>(&header.audioFormat), 2);
    outFile.write(reinterpret_cast<char*>(&header.numChannels), 2);
    outFile.write(reinterpret_cast<char*>(&header.sampleRate), 4);
    outFile.write(reinterpret_cast<char*>(&header.byteRate), 4);
    outFile.write(reinterpret_cast<char*>(&header.blockAlign), 2);
    outFile.write(reinterpret_cast<char*>(&header.bitsPerSample), 2);

    // Write data chunk
    outFile.write("data", 4);
    outFile.write(reinterpret_cast<char*>(&dataSize), 4);

    // Write actual audio data
    outFile.write(reinterpret_cast<char*>(interleavedData.data()), dataSize);

    outFile.close();

    std::cout << "Audio processed and saved to " << outputFile << "\n";
}
