@echo off
echo Building CHIP-8 Emulator...

REM Check if we have a build directory
if not exist build mkdir build

cd build

REM Run CMake
cmake ..
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b %errorlevel%
)

REM Build the project
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

echo.
echo Build completed successfully!
echo Executable is located in: build\Release\chip8_emulator.exe (or build\chip8_emulator.exe)
echo.
echo To run: chip8_emulator.exe path\to\rom.ch8
echo.
pause
