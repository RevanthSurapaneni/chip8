# Makefile for CHIP-8 Emulator
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lSDL2 -lSDL2main

# Source files
SOURCES = main.cpp Chip8.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = chip8_emulator

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean

# For Windows users with MinGW
windows:
	g++ -std=c++17 -Wall -Wextra -O2 main.cpp Chip8.cpp -o chip8_emulator.exe -lmingw32 -lSDL2main -lSDL2
