cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add executable
add_executable(chip8_emulator
    main.cpp
    Chip8.cpp
    Chip8.h
)

# Link libraries
target_link_libraries(chip8_emulator ${SDL2_LIBRARIES})
target_include_directories(chip8_emulator PRIVATE ${SDL2_INCLUDE_DIRS})

# For Windows, copy SDL2 DLLs if needed
if(WIN32)
    # This will copy SDL2.dll to the output directory
    if(TARGET SDL2::SDL2)
        add_custom_command(TARGET chip8_emulator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:chip8_emulator>)
    endif()
endif()
