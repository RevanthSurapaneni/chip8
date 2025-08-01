@echo off
echo Downloading SDL3 Development Libraries...
echo.

REM Check if curl is available
curl --version >nul 2>nul
if %errorlevel% neq 0 (
    echo curl not found. Please download SDL3 development libraries manually from:
    echo https://github.com/libsdl-org/SDL/releases
    echo.
    echo Look for: SDL3-devel-3.x.x-win32-x64.zip
    pause
    exit /b 1
)

echo Downloading SDL3 development libraries...
curl -L -o SDL3-devel.zip "https://github.com/libsdl-org/SDL/releases/download/preview-3.1.6/SDL3-devel-3.1.6-win32-x64.zip"

if %errorlevel% neq 0 (
    echo Download failed!
    pause
    exit /b %errorlevel%
)

echo Extracting SDL3 development libraries...
powershell -command "Expand-Archive -Path 'SDL3-devel.zip' -DestinationPath '.' -Force"

if %errorlevel% neq 0 (
    echo Extraction failed!
    pause
    exit /b %errorlevel%
)

echo Cleaning up...
del SDL3-devel.zip

echo.
echo SDL3 development libraries downloaded and extracted!
echo You can now run build_sdl3_dev.bat to compile the emulator.
echo.
pause
