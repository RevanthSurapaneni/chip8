@echo off
REM Build script for CHIP-8 Emulator SDL2 version
REM Compiles with g++ using SDL2 libraries

echo Building CHIP-8 Emulator (SDL2 version)...

REM Set SDL2 paths
set SDL2_INCLUDE=SDL2-2.30.9\include
set SDL2_LIB=SDL2-2.30.9\lib\x64

REM Compile with SDL2
g++ -std=c++11 -Wall -O2 -I"%SDL2_INCLUDE%" -o chip8_sdl2.exe Chip8.cpp main.cpp -L"%SDL2_LIB%" -lSDL2main -lSDL2

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Build successful! Created chip8_sdl2.exe
    echo.
    echo Usage: chip8_sdl2.exe ^<ROM_file^>
    echo Example: chip8_sdl2.exe "Tetris [Fran Dachille, 1991].ch8"
    echo.
    echo Controls:
    echo   CHIP-8 Key -^> PC Key
    echo   1 2 3 C    -^> 1 2 3 4
    echo   4 5 6 D    -^> Q W E R  
    echo   7 8 9 E    -^> A S D F
    echo   A 0 B F    -^> Z X C V
    echo   ESC to quit
    echo.
) else (
    echo ❌ Build failed!
    echo Check that:
    echo   1. g++ is installed and in PATH
    echo   2. SDL2 files are in SDL2-2.30.9/ directory
    echo   3. All source files exist
)

pause
