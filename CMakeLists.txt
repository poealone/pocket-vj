cmake_minimum_required(VERSION 3.16)
project(PocketVJ VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Platform detection ---
option(MUOS_BUILD "Cross-compile for MUOS/RG35XX (ARM)" OFF)

if(MUOS_BUILD)
    message(STATUS "Building for MUOS/RG35XX (ARM)")
    set(SCREEN_WIDTH 320)
    set(SCREEN_HEIGHT 240)
    set(TARGET_FPS 30)
    add_definitions(-DMUOS_BUILD -DSCREEN_W=${SCREEN_WIDTH} -DSCREEN_H=${SCREEN_HEIGHT} -DTARGET_FPS=${TARGET_FPS})
    # ARM cross-compile flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a53 -mfpu=neon-vfpv4 -mfloat-abi=hard")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a53 -mfpu=neon-vfpv4 -mfloat-abi=hard")
else()
    message(STATUS "Building for desktop (development)")
    set(SCREEN_WIDTH 640)
    set(SCREEN_HEIGHT 480)
    set(TARGET_FPS 60)
    add_definitions(-DSCREEN_W=${SCREEN_WIDTH} -DSCREEN_H=${SCREEN_HEIGHT} -DTARGET_FPS=${TARGET_FPS})
endif()

# --- Find SDL2 ---
find_package(SDL2 REQUIRED)

# --- Sources ---
set(SOURCES
    src/main.cpp
    src/engine/renderer.cpp
    src/engine/visual_node.cpp
    src/engine/nodes/waveform.cpp
    src/engine/nodes/bars.cpp
    src/engine/nodes/shapes.cpp
    src/engine/nodes/particles.cpp
    src/engine/nodes/color_field.cpp
    src/engine/audio/fft.cpp
    src/sequencer/pattern.cpp
    src/ui/tracker_view.cpp
    src/ui/preview.cpp
    src/ui/menu.cpp
    src/input.cpp
)

add_executable(pocket-vj ${SOURCES})

target_include_directories(pocket-vj PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(pocket-vj PRIVATE ${SDL2_LIBRARIES})

# --- Install ---
install(TARGETS pocket-vj DESTINATION bin)
install(DIRECTORY presets/ DESTINATION share/pocket-vj/presets)
install(DIRECTORY sprites/ DESTINATION share/pocket-vj/sprites)
