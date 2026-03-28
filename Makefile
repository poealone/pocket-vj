# Pocket VJ - Quick build for desktop development
# For MUOS cross-compilation, use CMake with -DMUOS_BUILD=ON

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = $(shell sdl2-config --libs)
INCLUDES = $(shell sdl2-config --cflags) -Isrc

SOURCES = \
	src/main.cpp \
	src/input.cpp \
	src/preset.cpp \
	src/engine/renderer.cpp \
	src/engine/visual_node.cpp \
	src/engine/param.cpp \
	src/engine/nodes/bars.cpp \
	src/engine/nodes/waveform.cpp \
	src/engine/nodes/shapes.cpp \
	src/engine/nodes/particles.cpp \
	src/engine/nodes/color_field.cpp \
	src/engine/audio/fft.cpp \
	src/sequencer/pattern.cpp \
	src/ui/tracker_view.cpp \
	src/ui/preview.cpp \
	src/ui/menu.cpp \
	src/ui/widgets.cpp \
	src/ui/node_editor.cpp \
	src/ui/node_browser.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TARGET = pocket-vj

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
