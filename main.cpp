// a command-line interface to the equaliser

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <fstream>
#include <cstdint>

#include "audio.h"


#define MAX 1024


void runReadFileCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);
void runWriteFileCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);
void runPrintHeaderCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);
void runDumpTxtCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);

void runGainCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);
void runEqualiseCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);
void runDynamicCompressionCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv);

struct Command {
    std::string code;
    void (*fn)(AudioProcessor&, int, std::vector<std::string>&);
    std::string argHint;
    std::string helpMsg;
};

static bool ECHO = false;
static Command* currCommand = nullptr;

static std::vector<Command> COMMANDS = {
    {"r", runReadFileCommand, "[input.wav]", "reads 16 bit .wav file"},
    {"w", runWriteFileCommand, "[output.wav]", "writes result to .wav file"},
    {"p", runPrintHeaderCommand, "", "prints header information of the .wav file"},
    {"d", runDumpTxtCommand, "[output.txt]", "prints audio data to .txt file"},
    
    {"g", runGainCommand, "g0 [sel]", "adds gain to audio data, sel = 'l', 'r', or 'b'"},
    {"eq", runEqualiseCommand, "g0 g1 g2 g3 g4 [sel]", "equalises based on 5 gains, sel = 'l', 'r', or 'b'"},
    {"drc", runDynamicCompressionCommand, "[thres] [ratio] [gain]", "dynamic range compression parameters: threshold, ratio, gain"},

    {"?", nullptr, "", "show this message"},
    {"q", nullptr, "", "quit"}
};

void showHelp() {
    std::cout << "Commands:" << '\n';
    for (const auto& cmd : COMMANDS) {
        std::cout << std::left << std::setw(5) << cmd.code << std::setw(25) << cmd.argHint << cmd.helpMsg << '\n';
    }
    std::cout << '\n';
}

void processOptions(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]...\n"
                 << "Options:\n"
                 << "    -h      show this help message\n"
                 << "    -e      echo - echo all commands\n";
            exit(EXIT_SUCCESS);
        } else if (arg == "-e") {
            ECHO = true;
        }
    }
}

void showWelcomeMessage() {
    std::cout << "Enter ? to see the list of commands." << '\n';
}

int getCommand(std::string& buf) {
    std::cout << "> ";
    if (!getline(std::cin, buf)) {
        return 0;
    }
    return 1;
}

std::vector<std::string> tokenize(const std::string& s) {
    std::vector<std::string> tokens;
    size_t start = 0, end;
    std::string delimiters = " \t\n";

    while ((end = s.find_first_of(delimiters, start)) != std::string::npos) {
        if (end != start) {
            tokens.push_back(s.substr(start, end - start));
        }
        start = end + 1;
    }

    if (start < s.size()) {
        tokens.push_back(s.substr(start));
    }

    return tokens;
}

int main(int argc, char* argv[]) {
    processOptions(argc, argv);
    showWelcomeMessage();

    AudioProcessor p1;

    bool done = false;
    std::string cmd;

    while (!done && getCommand(cmd)) {
        if (ECHO) {
            std::cout << cmd << '\n';
        }

        auto tokens = tokenize(cmd);

        std::string cmdName = tokens[0];

        if (cmdName == "?") {
            showHelp();
        } else if (cmdName == "q") {
            done = true;
        } else {
            bool validCommand = false;
            for (auto& command : COMMANDS) {
                if (cmdName == command.code) {
                    validCommand = true;
                    currCommand = &command;
                    if (command.fn) {
                        command.fn(p1, tokens.size(), tokens);
                    }
                    break;
                }
            }

            if (!validCommand) {
                std::cout << "Unknown command '" << cmdName << "'" << '\n';
            }
        }
    }

    return 0;
}



/*********************************** Commands *************************************/

void runReadFileCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (argc == 1) {
        std::string inputFile = "audio/royalty_44.1k_16bit_stereo.wav";
        //std::string inputFile = "audio/royalty_44.1k_16bit_mono.wav";
        //std::string inputFile = "audio/royalty_16k_16bit_stereo.wav";
        //std::string inputFile = "audio/royalty_16k_16bit_mono.wav";

        //std::string inputFile = "audio/super_shy_44.1k_16bit_stereo.wav";
        //std::string inputFile = "audio/super_shy_16k_16bit_stereo.wav";

        std::cout << "Default file read" << '\n';
        p.initialise(inputFile);
    } else if (argc == 2) {
        p.initialise(argv[1]);
    } else {
        std::cout << "Usage: r input.wav" << "\n\n";
        return;
    }
}

void runWriteFileCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (p.getLeftChannel().empty()) {
        std::cout << "Read in audio file with command \"r\" first! " << "\n\n";
        return;
    }

    if (argc == 1) {
        std::cout << "Default file write" << '\n';
        std::string outputFile = "audio/output.wav";
        p.writeOutputWav(outputFile);
    } else if (argc == 2) {
        p.writeOutputWav(argv[1]);
    } else {
        std::cout << "Usage: w output.wav" << "\n\n";
        return;
    }
}

void runPrintHeaderCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (p.getLeftChannel().empty()) {
        std::cout << "Read in audio file with command \"r\" first! " << "\n\n";
        return;
    }
    p.printWavHeader();
}

void runDumpTxtCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (p.getLeftChannel().empty()) {
        std::cout << "Read in audio file with command \"r\" first! " << "\n\n";
        return;
    }

    if (argc == 1) {
        std::cout << "Default file write" << '\n';
        std::string outputFile = "audio/rawDump.txt";
        p.writeOutputTxt(outputFile);
    } else if (argc == 2) {
        p.writeOutputTxt(argv[1]);
    } else {
        std::cout << "Usage: d output.txt" << "\n\n";
        return;
    }
}

void runGainCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: g g0 [sel]" << "\n\n";
        return;
    }

    if (p.getLeftChannel().empty()) {
        std::cout << "Read in audio file with command \"r\" first! " << "\n\n";
        return;
    }

    char sel = 'b';
    if (argc == 3) sel = tolower(argv[2][0]);

    float gain;
    try {
        gain = stof(argv[1]);
    } catch (std::exception& e) {
        std::cout << "Error: Invalid value " << argv[1] << "\n\n";
        return;
    }

    volumeGain(p, gain, sel);
}

void runEqualiseCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (argc != 6 && argc != 7) {
        std::cout << "Usage: eq g0 g1 g2 g3 g4 [sel]" << "\n\n";
        return;
    }

    if (p.getLeftChannel().empty()) {
        std::cout << "Read in audio file with command \"r\" first!" << "\n\n";
        return;
    }

    char sel = 'b';
    if (argc == 7) sel = tolower(argv[6][0]);

    std::vector<float> gains;
    for (int i = 1; i < 6; i++) {
        try {
            float num = stof(argv[i]);
            gains.push_back(num);
        } catch (std::exception& e) {
            std::cout << "Error: Invalid value " << argv[i] << "\n\n";
            return;
        }
    }
    equaliser(p, gains, sel);
}

void runDynamicCompressionCommand(AudioProcessor& p, int argc, std::vector<std::string>& argv) {
    if (argc > 4) {
        std::cout << "Usage: drc [thres] [ratio] [gain]" << "\n\n";
        return;
    }

    if (p.getLeftChannel().empty()) {
        std::cout << "Read in audio file with command \"r\" first!" << "\n\n";
        return;
    }

    float threshold = 0.7f;
    if (argc >= 2) {
        try {
            threshold = stof(argv[1]);
        } catch (std::exception& e) {
            std::cout << "Error: Invalid threshold value " << argv[1] << "\n\n";
            return;
        }
    }

    int ratio = 2;
    if (argc >= 3) {
        try {
            ratio = stoi(argv[2]);
        } catch (std::exception& e) {
            std::cout << "Error: Invalid ratio value " << argv[2] << "\n\n";
            return;
        }
    }
    
    float makeUpGain = 1.0f;
    if (argc >= 4) {
        try {
            makeUpGain = stof(argv[3]);
        } catch (std::exception& e) {
            std::cout << "Error: Invalid make-up gain value " << argv[3] << "\n\n";
            return;
        }
    }
    
    dynamicCompression(p, threshold, ratio, makeUpGain);
}
