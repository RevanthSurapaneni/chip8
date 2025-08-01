@echo off
echo Building CHIP-8 Emulator with SDL3 Development Libraries...

REM Look for SDL3 development folder
set SDL3_DEV_PATH=
for /d %%i in (SDL3-devel-*) do set SDL3_DEV_PATH=%%i

if "%SDL3_DEV_PATH%"=="" (
    echo Error: SDL3 development folder not found!
    echo Please run download_sdl3.bat first or manually download SDL3 development libraries.
    pause
    exit /b 1
)

echo Found SDL3 development libraries: %SDL3_DEV_PATH%
echo.

REM Check if include and lib directories exist
if not exist "%SDL3_DEV_PATH%\include" (
    echo Error: Include directory not found in %SDL3_DEV_PATH%
    pause
    exit /b 1
)

if not exist "%SDL3_DEV_PATH%\lib" (
    echo Error: Lib directory not found in %SDL3_DEV_PATH%
    pause
    exit /b 1
)

echo Compiling with SDL3...
g++ -std=c++17 -Wall -Wextra -O2 ^
    -I"%SDL3_DEV_PATH%\include" ^
    main.cpp Chip8.cpp ^
    -o chip8_emulator.exe ^
    -L"%SDL3_DEV_PATH%\lib\x64" ^
    -lSDL3main -lSDL3

if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)

REM Copy SDL3.dll to current directory
if exist "%SDL3_DEV_PATH%\lib\x64\SDL3.dll" (
    copy "%SDL3_DEV_PATH%\lib\x64\SDL3.dll" . >nul
    echo SDL3.dll copied from development libraries.
) else if exist "SDL3-3.2.16-win32-x64\SDL3.dll" (
    copy "SDL3-3.2.16-win32-x64\SDL3.dll" . >nul
    echo SDL3.dll copied from runtime package.
) else (
    echo Warning: SDL3.dll not found. You may need to copy it manually.
)

echo.
echo Build completed successfully!
echo.
echo To run: chip8_emulator.exe "ROM_FILE.ch8"
echo Example: chip8_emulator.exe "Tetris [Fran Dachille, 1991].ch8"
echo.
pause
