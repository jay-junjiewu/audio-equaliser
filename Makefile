CC = clang
CFLAGS = -Wall -Wvla -Werror -g
CXX = clang++
CXXFLAGS = -Wall -Wvla -Werror -g 

SRC = main.cpp audio.cpp dsp.cpp
OBJ = $(SRC:.cpp=.o)

########################################################################

.PHONY: all clean asan msan nosan

asan: CFLAGS += -fsanitize=address,leak,undefined
asan: CXXFLAGS += -fsanitize=address,leak,undefined
asan: all

msan: CFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins
msan: CXXFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins
msan: all

nosan: all

########################################################################

all: program

program: $(OBJ)
	$(CXX) $(CXXFLAGS) -o program $(OBJ)
	rm -f $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

########################################################################

clean:
	rm -f $(OBJ) program
