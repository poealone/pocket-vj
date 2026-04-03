# Pocket VJ - Quick build for desktop development
# For MUOS cross-compilation, use CMake with -DMUOS_BUILD=ON

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = $(shell sdl2-config --libs)
INCLUDES = $(shell sdl2-config --cflags) -Isrc

# SDL2_mixer — link when available (ARM target has it; desktop may not)
SDL2_MIXER_LIBS := $(shell pkg-config --libs SDL2_mixer 2>/dev/null)
ifneq ($(SDL2_MIXER_LIBS),)
LDFLAGS  += $(SDL2_MIXER_LIBS)
endif

SOURCES = \
	src/main.cpp \
	src/input.cpp \
	src/preset.cpp \
	src/engine/renderer.cpp \
	src/engine/visual_node.cpp \
	src/engine/param.cpp \
	src/engine/layer.cpp \
	src/engine/sprite.cpp \
	src/engine/nodes/bars.cpp \
	src/engine/nodes/waveform.cpp \
	src/engine/nodes/shapes.cpp \
	src/engine/nodes/particles.cpp \
	src/engine/nodes/color_field.cpp \
	src/engine/nodes/noise.cpp \
	src/engine/nodes/laser.cpp \
	src/engine/nodes/strobe.cpp \
	src/engine/nodes/grid.cpp \
	src/engine/nodes/tunnel.cpp \
	src/engine/nodes/starfield.cpp \
	src/engine/nodes/plasma.cpp \
	src/engine/nodes/mirror.cpp \
	src/engine/nodes/blur.cpp \
	src/engine/nodes/feedback.cpp \
	src/engine/nodes/glitch.cpp \
	src/engine/nodes/scanline.cpp \
	src/engine/nodes/pixelate.cpp \
	src/engine/nodes/colorshift.cpp \
	src/engine/nodes/edge.cpp \
	src/engine/audio/fft.cpp \
	src/engine/audio/audio_input.cpp \
	src/engine/modulators/lfo.cpp \
	src/engine/modulators/envelope.cpp \
	src/engine/modulators/audio_mod.cpp \
	src/engine/3d/rasterizer.cpp \
	src/engine/3d/obj_loader.cpp \
	src/engine/3d/camera3d.cpp \
	src/engine/nodes/mesh.cpp \
	src/engine/nodes/cube.cpp \
	src/engine/nodes/sphere.cpp \
	src/engine/nodes/torus.cpp \
	src/sequencer/pattern.cpp \
	src/ui/tracker_view.cpp \
	src/ui/preview.cpp \
	src/ui/menu.cpp \
	src/ui/widgets.cpp \
	src/ui/node_editor.cpp \
	src/ui/node_browser.cpp \
	src/ui/layer_editor.cpp \
	src/ui/file_browser.cpp \
	src/ui/music_browser.cpp \
	src/ui/performance.cpp \
	src/audio/music_player.cpp \
	src/engine/scene.cpp \
	src/engine/bpm.cpp \
	src/engine/recorder.cpp \
	src/export/pdviz.cpp

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
