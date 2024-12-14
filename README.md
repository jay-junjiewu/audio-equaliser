# audio-equaliser


This personal project is a Digital Audio Equaliser written purely using C++ STL (no external libraries) for processing audio signals. The equaliser reads in a `.wav` audio file and allows users to apply various gains to 5 frequency bands (sub-bass, bass, midrange, upper midrange, treble). It can also process stereo audio and save the result into a `.wav` file.

Note: I could have used any of the many good libraries out there, which are possibly even more efficient in terms of implementation than my code, or I could have written it all in Python. But I chose to write it all from scratch in C++ to have full control over the implementation details and to gain a better understanding of signal processing and C++ STL.

---

## Features

- **5-Band Equaliser:**
  - Adjust individual gain (in dB) for 5 frequency bands.
- **Filtering:**
  - Applies band-pass filters to isolate and process specific frequency ranges.
- **Zero Phase:**
  - Achieves zero phase filtering by processing filtering in both the forward and reverse directions.
- **Stereo Processing:**
  - Supports separate equalisation for left and right audio channels.
- **File Format Support:**
    - Allows 16-bit depth `.wav` files with any sampling frequency, both stereo and mono.


## Usage
1. Clone this repository:
   ```bash
   git clone https://github.com/jay-junjiewu/audio-equaliser.git
   cd audio-equaliser
   ```
2. Build the project using the included Makefile:
   ```bash
   make
   ```
3. Run the program:
   ```bash
   ./program
   ```


## How It Works

1. **Audio Input**:
   - Reads a `.wav` audio file into memory.
2. **Filter**:
   - Applies filters to isolate specific frequency ranges.
3. **Gain Adjustment**:
   - Scales each band by a specified gain factor (in dB).
4. **Audio Output**:
   - Combines the processed frequency bands and outputs a `.wav` file.


## Contributing
Contributions are welcome! Please fork the repository and submit a pull request.

Feel free to reach out with questions or suggestions!


## Future Enhancement Ideas
- Add support for real-time audio processing.
- Optimise performance for large audio files, like processing chunks separately.
- Support `.wav` files for all bit depths.
- Support reading/writing different files like `.mp3`, `.aiff`, `.flac` etc.
- Support multichannel audio processing.

