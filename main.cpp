#include "Chip8.h"
#include <SDL.h>  //magic (error handled in build batch file)
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 512;
const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;

// Audio specifications
const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 3000;
const int FREQUENCY = 440; // 440 Hz tone (A4 note)

// Audio data structure
struct AudioData {
    double phase;
    bool playing;
    AudioData() : phase(0.0), playing(false) {}
};

AudioData audioData;

// Audio callback function - generates square wave
void audioCallback(void* userdata, uint8_t* stream, int len) {
    AudioData* audio = static_cast<AudioData*>(userdata);
    int16_t* audioBuffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);
    
    for (int i = 0; i < samples; ++i) {
        if (audio->playing) {
            // Generate square wave
            double sineValue = sin(audio->phase);
            int16_t sample = (sineValue > 0) ? AMPLITUDE : -AMPLITUDE;
            audioBuffer[i] = sample;
            
            // Update phase
            audio->phase += 2.0 * M_PI * FREQUENCY / SAMPLE_RATE;
            if (audio->phase >= 2.0 * M_PI) {
                audio->phase -= 2.0 * M_PI;
            }
        } else {
            audioBuffer[i] = 0; // Silence
        }
    }
}

// Key mapping for keypad
uint8_t keymap[16] = {
    SDLK_x,    // 0
    SDLK_1,    // 1
    SDLK_2,    // 2
    SDLK_3,    // 3
    SDLK_q,    // 4
    SDLK_w,    // 5
    SDLK_e,    // 6
    SDLK_a,    // 7
    SDLK_s,    // 8
    SDLK_d,    // 9
    SDLK_z,    // A
    SDLK_c,    // B
    SDLK_4,    // C
    SDLK_r,    // D
    SDLK_f,    // E
    SDLK_v     // F
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }    // Create window
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }// Create texture for display
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    if (texture == nullptr) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;    }

    // Setup audio
    SDL_AudioSpec desiredSpec, obtainedSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = SAMPLE_RATE;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 1024;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = &audioData;
    
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, 0);
    if (audioDevice == 0) {
        std::cerr << "Warning: Could not open audio device! SDL_Error: " << SDL_GetError() << std::endl;
        std::cerr << "Continuing without sound..." << std::endl;
    } else {
        SDL_PauseAudioDevice(audioDevice, 0); // Start audio playback (bugged currently :(   )
    }

    // Initialize CHIP-8 system and load ROM
    Chip8 chip8;
    chip8.loadRom(argv[1]);    // Main loop
    bool quit = false;
    SDL_Event e;    auto lastTime = std::chrono::high_resolution_clock::now();
    const std::chrono::microseconds targetFrameTime(45000); //speed
    const int instructionsPerFrame = 5; 

    std::cout << "CHIP-8 Emulator Controls:" << std::endl;
    std::cout << "CHIP-8 Key -> PC Key" << std::endl;
    std::cout << "1 2 3 C -> 1 2 3 4" << std::endl;
    std::cout << "4 5 6 D -> Q W E R" << std::endl;
    std::cout << "7 8 9 E -> A S D F" << std::endl;
    std::cout << "A 0 B F -> Z X C V" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;

    while (!quit) {        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);// Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                
                // Handle keys
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.setKey(i, true);
                        break;
                    }
                }
            }
            else if (e.type == SDL_KEYUP) {
                // Handle keys
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.setKey(i, false);
                        break;
                    }
                }
            }        }        // Execute single cycle
        for (int i = 0; i < instructionsPerFrame; ++i) {
            chip8.cycle();
        }// Update display if draw flag is set
        if (chip8.drawFlag) {
            // Convert CHIP-8 display to SDL texture format
            uint32_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT];
            for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; ++i) {
                pixels[i] = chip8.display[i] ? 0xFFFFFFFF : 0xFF000000; // White or black
            }
            
            // Update texture with display data
            if (SDL_UpdateTexture(texture, nullptr, pixels, DISPLAY_WIDTH * sizeof(uint32_t)) == 0) {
                // Clear screen
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                // Render texture
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);

                // Update screen
                SDL_RenderPresent(renderer);
            }
            
            chip8.drawFlag = false;        }        // Handle sound - continuous while sound timer > 0
        if (audioDevice != 0) {
            audioData.playing = chip8.shouldPlaySound();
        } else if (chip8.soundFlag) {
            // Fallback to console beep if audio failed to initialize
            std::cout << '\a';
            chip8.soundFlag = false;
        }

        // Frame rate limiting
        if (deltaTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - deltaTime);
        }
        lastTime = currentTime;    }

    // Cleanup
    if (audioDevice != 0) {
        SDL_CloseAudioDevice(audioDevice);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
