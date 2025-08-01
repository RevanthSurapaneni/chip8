#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <cstring>

// CHIP-8 fontset (each character is 4x5 pixels)
uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() : gen(rd()), dis(0, 255), debugMode(false) {
    initialize();
}

void Chip8::initialize() {
    pc = 0x200;         // Program counter starts at 0x200
    I = 0;              // Reset index register
    sp = 0;             // Reset stack pointer
    
    // Clear display
    memset(gfx, 0, sizeof(gfx));
    memset(display, 0, sizeof(display));
    
    // Clear stack
    memset(stack, 0, sizeof(stack));
    
    // Clear registers V0-VF
    memset(V, 0, sizeof(V));
    
    // Clear memory
    memset(memory, 0, sizeof(memory));
    
    // Clear keys
    memset(key, 0, sizeof(key));
    
    // Load fontset
    loadFontset();
    
    // Reset timers
    delay_timer = 0;
    sound_timer = 0;
    
    // Reset flags
    drawFlag = false;
    soundFlag = false;
}

void Chip8::loadFontset() {
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontset[i];
    }
}

void Chip8::loadRom(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open ROM file " << filename << std::endl;
        return;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (size <= (4096 - 512)) {
        file.read(reinterpret_cast<char*>(memory + 512), size);
        std::cout << "ROM loaded successfully: " << filename << " (" << size << " bytes)" << std::endl;
    } else {
        std::cerr << "Error: ROM too large for memory" << std::endl;
    }
    
    file.close();
}

void Chip8::cycle() {
    // Fetch opcode
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    
    if (debugMode) {
        std::cout << "PC: 0x" << std::hex << pc << " Opcode: 0x" << opcode << std::dec << std::endl;
    }
    
    // Decode and execute
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // 0x00E0: Clear display
                    memset(gfx, 0, sizeof(gfx));
                    memset(display, 0, sizeof(display));
                    drawFlag = true;
                    pc += 2;
                    break;
                    
                case 0x000E: // 0x00EE: Return from subroutine
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    pc += 2;
            }
            break;
            
        case 0x1000: // 0x1NNN: Jump to address NNN
            pc = opcode & 0x0FFF;
            break;
            
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
            
        case 0x3000: // 0x3XNN: Skip next instruction if VX equals NN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
            
        case 0x4000: // 0x4XNN: Skip next instruction if VX doesn't equal NN
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
            
        case 0x5000: // 0x5XY0: Skip next instruction if VX equals VY
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;
            
        case 0x6000: // 0x6XNN: Set VX to NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
            
        case 0x7000: // 0x7XNN: Add NN to VX
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
            
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: // 0x8XY0: Set VX to the value of VY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0001: // 0x8XY1: Set VX to VX or VY
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0002: // 0x8XY2: Set VX to VX and VY
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0003: // 0x8XY3: Set VX to VX xor VY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    
                case 0x0004: // 0x8XY4: Add VY to VX, VF = carry
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        uint8_t y = (opcode & 0x00F0) >> 4;
                        uint16_t sum = V[x] + V[y];
                        V[0xF] = (sum > 255) ? 1 : 0;
                        V[x] = sum & 0xFF;
                    }
                    pc += 2;
                    break;
                    
                case 0x0005: // 0x8XY5: Subtract VY from VX, VF = NOT borrow
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        uint8_t y = (opcode & 0x00F0) >> 4;
                        V[0xF] = (V[x] > V[y]) ? 1 : 0;
                        V[x] -= V[y];
                    }
                    pc += 2;
                    break;
                    
                case 0x0006: // 0x8XY6: Shift VX right by one, VF = LSB
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        V[0xF] = V[x] & 0x1;
                        V[x] >>= 1;
                    }
                    pc += 2;
                    break;
                    
                case 0x0007: // 0x8XY7: Set VX to VY minus VX, VF = NOT borrow
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        uint8_t y = (opcode & 0x00F0) >> 4;
                        V[0xF] = (V[y] > V[x]) ? 1 : 0;
                        V[x] = V[y] - V[x];
                    }
                    pc += 2;
                    break;
                    
                case 0x000E: // 0x8XYE: Shift VX left by one, VF = MSB
                    {
                        uint8_t x = (opcode & 0x0F00) >> 8;
                        V[0xF] = V[x] >> 7;
                        V[x] <<= 1;
                    }
                    pc += 2;
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    pc += 2;
            }
            break;
            
        case 0x9000: // 0x9XY0: Skip next instruction if VX doesn't equal VY
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;
            
        case 0xA000: // 0xANNN: Set I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
            
        case 0xB000: // 0xBNNN: Jump to the address NNN plus V0
            pc = (opcode & 0x0FFF) + V[0];
            break;
            
        case 0xC000: // 0xCXNN: Set VX to a random number masked by NN
            V[(opcode & 0x0F00) >> 8] = getRandom() & (opcode & 0x00FF);
            pc += 2;
            break;
            
        case 0xD000: // 0xDXYN: Draw sprite at (VX, VY) with N bytes of sprite data starting at I
            {
                uint8_t x = V[(opcode & 0x0F00) >> 8];
                uint8_t y = V[(opcode & 0x00F0) >> 4];
                uint8_t height = opcode & 0x000F;
                uint8_t pixel;
                
                V[0xF] = 0;
                for (int yline = 0; yline < height; yline++) {
                    pixel = memory[I + yline];
                    for (int xline = 0; xline < 8; xline++) {
                        if ((pixel & (0x80 >> xline)) != 0) {
                            int px = (x + xline) % 64;
                            int py = (y + yline) % 32;
                            if (gfx[px + py * 64] == 1)
                                V[0xF] = 1;
                            gfx[px + py * 64] ^= 1;
                        }
                    }
                }
                
                // Update display buffer
                for (int i = 0; i < 64 * 32; ++i) {
                    display[i] = gfx[i] ? 0xFFFFFFFF : 0x00000000;
                }
                
                drawFlag = true;
                pc += 2;
            }
            break;
            
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: // 0xEX9E: Skip next instruction if key stored in VX is pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;
                    
                case 0x00A1: // 0xEXA1: Skip next instruction if key stored in VX isn't pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 4;
                    else
                        pc += 2;
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    pc += 2;
            }
            break;
            
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // 0xFX07: Set VX to the value of the delay timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                    
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in VX
                    {
                        bool keyPressed = false;
                        for (int i = 0; i < 16; ++i) {
                            if (key[i] != 0) {
                                V[(opcode & 0x0F00) >> 8] = i;
                                keyPressed = true;
                                break;
                            }
                        }
                        if (!keyPressed)
                            return; // Don't increment pc, wait for key press
                        pc += 2;
                    }
                    break;
                    
                case 0x0015: // 0xFX15: Set the delay timer to VX
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x0018: // 0xFX18: Set the sound timer to VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x001E: // 0xFX1E: Add VX to I
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    
                case 0x0029: // 0xFX29: Set I to the location of the sprite for character in VX
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;
                    
                case 0x0033: // 0xFX33: Store binary-coded decimal representation of VX at I, I+1, I+2
                    {
                        uint8_t value = V[(opcode & 0x0F00) >> 8];
                        memory[I] = value / 100;
                        memory[I + 1] = (value / 10) % 10;
                        memory[I + 2] = (value % 100) % 10;
                    }
                    pc += 2;
                    break;
                    
                case 0x0055: // 0xFX55: Store registers V0 through VX in memory starting at location I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[I + i] = V[i];
                    pc += 2;
                    break;
                    
                case 0x0065: // 0xFX65: Read registers V0 through VX from memory starting at location I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        V[i] = memory[I + i];
                    pc += 2;
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    pc += 2;
            }
            break;
            
        default:
            std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
            pc += 2;
    }
    
    // Update timers
    if (delay_timer > 0)
        --delay_timer;
        
    if (sound_timer > 0) {
        if (sound_timer == 1)
            soundFlag = true;
        --sound_timer;
    }
}

void Chip8::setKey(int key, bool pressed) {
    if (key >= 0 && key < 16) {
        this->key[key] = pressed ? 1 : 0;
    }
}

uint8_t Chip8::getRandom() {
    return static_cast<uint8_t>(dis(gen));
}

bool Chip8::shouldPlaySound() const {
    return sound_timer > 0;
}
