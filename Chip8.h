#pragma once
#include <cstdint>
#include <random>

class Chip8 {
public:
    Chip8();    void loadRom(const char* filename);
    void cycle();    void setKey(int key, bool pressed);
    bool shouldPlaySound() const;  // Check if sound should be playing
    void enableDebugMode(bool enabled) { debugMode = enabled; } // Enable debug output
    
    // Public members for display and audio
    uint32_t display[64 * 32];  // 64x32 pixel display
    bool drawFlag;
    bool soundFlag;
    
private:
    // CPU registers
    uint8_t V[16];              // 16 8-bit registers V0-VF
    uint16_t I;                 // Index register
    uint16_t pc;                // Program counter
    uint8_t sp;                 // Stack pointer
    uint16_t stack[16];         // Stack
    
    // Memory and graphics
    uint8_t memory[4096];       // 4K memory
    uint8_t gfx[64 * 32];       // Graphics buffer
    
    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    // Input
    uint8_t key[16];            // Keypad state
    
    // Random number generator
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
      // Helper methods
    void initialize();
    void loadFontset();
    uint8_t getRandom();
    
    // Debug mode
    bool debugMode;
};