#include "Chip8.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h> // For Windows console input

const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;

void printDisplay(const uint32_t* display) {
    system("cls"); // Clear console
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            if (display[x + y * DISPLAY_WIDTH] != 0) {
                std::cout << "█";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

void printControls() {
    std::cout << "\nCHIP-8 Console Emulator Controls:" << std::endl;
    std::cout << "CHIP-8 Key -> PC Key" << std::endl;
    std::cout << "1 2 3 C -> 1 2 3 4" << std::endl;
    std::cout << "4 5 6 D -> Q W E R" << std::endl;
    std::cout << "7 8 9 E -> A S D F" << std::endl;
    std::cout << "A 0 B F -> Z X C V" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }    // Initialize CHIP-8 system and load ROM
    Chip8 chip8;
    // chip8.enableDebugMode(true); // Disabled debug output for normal speed
    chip8.loadRom(argv[1]);

    printControls();    // Main loop
    bool quit = false;    auto lastTime = std::chrono::high_resolution_clock::now();
    const std::chrono::milliseconds targetFrameTime(16); // ~60 FPS (16.67ms per frame) for normal speed
    const int instructionsPerFrame = 10; // Execute multiple instructions per frame for normal speed

    while (!quit) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);

        // Handle keyboard input (non-blocking)
        if (_kbhit()) {
            char key = _getch();
            
            // Convert to lowercase
            if (key >= 'A' && key <= 'Z') {
                key = key + ('a' - 'A');
            }
            
            // Check for quit
            if (key == 27) { // ESC key
                quit = true;
                continue;
            }
            
            // Map keys to CHIP-8 keypad
            int chip8Key = -1;
            switch (key) {
                case '1': chip8Key = 0x1; break;
                case '2': chip8Key = 0x2; break;
                case '3': chip8Key = 0x3; break;
                case '4': chip8Key = 0xC; break;
                case 'q': chip8Key = 0x4; break;
                case 'w': chip8Key = 0x5; break;
                case 'e': chip8Key = 0x6; break;
                case 'r': chip8Key = 0xD; break;
                case 'a': chip8Key = 0x7; break;
                case 's': chip8Key = 0x8; break;
                case 'd': chip8Key = 0x9; break;
                case 'f': chip8Key = 0xE; break;
                case 'z': chip8Key = 0xA; break;
                case 'x': chip8Key = 0x0; break;
                case 'c': chip8Key = 0xB; break;
                case 'v': chip8Key = 0xF; break;
            }
            
            if (chip8Key != -1) {
                chip8.setKey(chip8Key, true);
                // Simulate key release after a short time
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                chip8.setKey(chip8Key, false);
            }        }        // Execute single CHIP-8 cycle for slow debugging
        for (int i = 0; i < instructionsPerFrame; ++i) {
            chip8.cycle();
        }

        // Update display if draw flag is set
        if (chip8.drawFlag) {
            printDisplay(chip8.display);
            chip8.drawFlag = false;
        }

        // Handle sound
        if (chip8.soundFlag) {
            std::cout << '\a'; // Bell character for system beep
            chip8.soundFlag = false;
        }

        // Frame rate limiting
        if (deltaTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - deltaTime);
        }
        lastTime = currentTime;
    }

    return 0;
}
