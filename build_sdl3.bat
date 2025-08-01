@echo off
echo Building CHIP-8 Emulator with SDL3...

REM Set SDL3 path
set SDL3_PATH=SDL3-3.2.16-win32-x64

REM Check if SDL3 folder exists
if not exist "%SDL3_PATH%" (
    echo Error: SDL3 folder not found!
    echo Expected: %SDL3_PATH%
    pause
    exit /b 1
)

echo Compiling with SDL3...
g++ -std=c++17 -Wall -Wextra -O2 ^
    -I"%SDL3_PATH%\include" ^
    main.cpp Chip8.cpp ^
    -o chip8_emulator.exe ^
    -L"%SDL3_PATH%\lib\x64" ^
    -lSDL3

if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)

REM Copy SDL3.dll to current directory
copy "%SDL3_PATH%\SDL3.dll" . >nul

echo.
echo Build completed successfully!
echo SDL3.dll copied to current directory.
echo.
echo To run: chip8_emulator.exe "ROM_FILE.ch8"
echo Example: chip8_emulator.exe "Tetris [Fran Dachille, 1991].ch8"
echo.
pause
