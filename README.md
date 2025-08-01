<img width="1282" height="673" alt="image" src="https://github.com/user-attachments/assets/d66c2a53-fdbf-401e-abb3-06b37c79f641" />



# CHIP-8 Emulator

A fully functional CHIP-8 emulator written in C++ with SDL2.

## Features

- Complete CHIP-8 instruction set implementation
- 64x32 pixel display with proper scaling
- Sound support (system beep)
- Keyboard input mapping
- ROM loading capability
- Proper timing and frame rate control

## Requirements

- C++17 compatible compiler (GCC, Clang, MSVC)
- SDL2 development libraries
- CMake (optional, for building with CMake)

## Building

### Console Version (No Dependencies Required)

If you don't have SDL2 installed, you can build and run the console version:

```bash
g++ -std=c++17 -O2 main_console.cpp Chip8.cpp -o chip8_console.exe
```

### SDL2 Version (Full Graphics)

First install SDL2, then:

#### Using CMake (Recommended)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Using Makefile (Linux/macOS)

```bash
make
```

#### Using Makefile (Windows with MinGW)

```bash
make windows
```

#### Manual compilation

```bash
g++ -std=c++17 -O2 main.cpp Chip8.cpp -o chip8_emulator -lSDL2 -lSDL2main
```

## Installing SDL2

### Windows
1. Download SDL2 development libraries from https://www.libsdl.org/
2. Extract to a folder (e.g., C:\SDL2)
3. Add SDL2\bin to your PATH
4. Set environment variables or use vcpkg

### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev
```

### macOS (with Homebrew)
```bash
brew install sdl2
```

### Arch Linux
```bash
sudo pacman -S sdl2
```

## Usage

### Quick Start (Windows)

#### Console Version (No Dependencies)
```cmd
build.bat
chip8_console.exe "Tetris [Fran Dachille, 1991].ch8"
```

#### SDL2 Graphics Version
```cmd
build_sdl2.bat
chip8_sdl2.exe "Tetris [Fran Dachille, 1991].ch8"
```

### General Usage

### Console Version
```bash
./chip8_console.exe <rom_file>
```

### SDL2 Version  
```bash
./chip8_emulator <rom_file>
```

Example:
```bash
./chip8_console.exe "Tetris [Fran Dachille, 1991].ch8"
```

## Controls

The CHIP-8 had a 16-key hexadecimal keypad. The keys are mapped as follows:

```
CHIP-8 Keypad    PC Keyboard
1 2 3 C          1 2 3 4
4 5 6 D          Q W E R
7 8 9 E          A S D F
A 0 B F          Z X C V
```

Additional controls:
- **ESC**: Quit emulator

## CHIP-8 Technical Specifications

- **Memory**: 4KB (4096 bytes)
- **Display**: 64x32 pixels monochrome
- **Registers**: 16 8-bit general purpose registers (V0-VF)
- **Stack**: 16 levels of nesting
- **Timers**: 60Hz delay and sound timers
- **Input**: 16-key hexadecimal keypad

## ROM Files

You can find CHIP-8 ROM files (.ch8) from various sources online. Popular games include:
- Pong
- Tetris
- Space Invaders
- Breakout
- Snake

Make sure to only use ROM files that you have the legal right to use.

## Implementation Details

This emulator implements the complete CHIP-8 instruction set:

- **0x0nnn**: System calls (clear screen, return)
- **0x1nnn**: Jump
- **0x2nnn**: Call subroutine
- **0x3xnn - 0x5xy0**: Conditional skips
- **0x6xnn**: Set register
- **0x7xnn**: Add to register
- **0x8xyn**: Arithmetic and logic operations
- **0x9xy0**: Skip if registers not equal
- **0xAnnn**: Set index register
- **0xBnnn**: Jump with offset
- **0xCxnn**: Random number generation
- **0xDxyn**: Draw sprite
- **0xExnn**: Key input operations
- **0xFxnn**: Timer operations, BCD conversion, memory operations

## Troubleshooting

### SDL2 not found
Make sure SDL2 development libraries are installed and in your system's library path.

### ROM won't load
- Check that the ROM file exists and is readable
- Ensure the ROM file is a valid CHIP-8 program (.ch8 extension)
- Maximum ROM size is 3584 bytes (4096 - 512 for system area)

### Display issues
The emulator scales the 64x32 display to 1024x512 pixels. If you have display issues, try running in windowed mode.

## License

This project is open source. Feel free to modify and distribute according to your needs.

