# 🎨 Pocket VJ

**Visual production app for MUOS / RG35XX.** Create, sequence, and perform visuals on your handheld.

Companion to [Pocket DAW](https://github.com/poealone/pocket-daw). Exports `.pdviz` presets that Pocket DAW imports as synth visualizers.

---

## Features

### 27 Visual Node Types

| Category | Nodes |
|----------|-------|
| **Source** | Spectrum Bars, Waveform, Shapes, Particles, Color Field, Noise, Laser, Strobe, Grid, Tunnel, Starfield, Plasma |
| **FX** | Mirror, Blur, Feedback/Trails, Glitch, Scanlines, Pixelate, Color Shift, Edge Detect |
| **Modulators** | LFO, ADSR Envelope, Audio Reactive |
| **3D** | Mesh (.OBJ loader), Cube, Sphere, Torus |

### Visual Engine
- **8-layer system** with 5 blend modes (Normal, Add, Multiply, Screen, Overlay)
- **Per-layer opacity**, mute, and solo
- **Real FFT audio analysis** with beat detection
- **Custom sprite loading** for particle effects (.BMP with transparency)
- **Software 3D rasterizer** with Z-buffer and flat shading
- **.OBJ model loader** for custom 3D meshes

### Creation Tools
- **Parameter system** — every value is tweakable via sliders, knobs, and toggles
- **Node browser** — add any of 27 node types to any layer
- **Preset save/load** — `.pvj` JSON format
- **16-step pattern sequencer** with BPM sync

### Performance Mode
- **Live FX control** — 3 assignable FX slots with real-time sliders
- **8 scene slots** — save/load entire visual states, crossfade between them
- **BPM tap tempo** — tap Start to sync visuals to your beat
- **Particle burst trigger** — fire particle effects on demand
- **Frame recording** — capture 10 seconds of RLE-compressed output

### Integration
- **.pdviz export** — simplified format for Pocket DAW synth visualizers
- **Custom assets** — drop `.bmp` sprites in `sprites/` and `.obj` meshes in `meshes/`

---

## Installation

### From Archive Manager (Recommended)

1. Download `pocket-vj-v1.0.0.muxupd` from [Releases](https://github.com/poealone/pocket-vj/releases)
2. Copy to `/mnt/mmc/ARCHIVE/` on your SD card
3. On device: Archive Manager → select file → press A
4. Launch from Applications menu

### Manual Install

Copy the following to `/mnt/mmc/MUOS/application/PocketVJ/`:
```
PocketVJ/
├── pocket-vj          # ARM64 binary
├── mux_launch.sh      # MUOS launcher
├── presets/            # Visual presets (.pvj)
├── sprites/            # Custom particle sprites (.bmp)
│   ├── particles/
│   ├── backgrounds/
│   └── overlays/
└── meshes/             # 3D models (.obj)
    ├── crystal.obj
    └── pyramid.obj
```

---

## Controls

### Main View (Tracker)

| Button | Action |
|--------|--------|
| **D-Pad** | Navigate |
| **A** | Open layer editor |
| **B** | Back |
| **X** | Add new node (node browser) |
| **Y** | Fullscreen preview |
| **L / R** | Switch layer |
| **Start** | Play/Stop sequencer |
| **Select** | Open menu |
| **L2** | Toggle recording |
| **Select+Start** | Enter performance mode |

### Layer Editor

| Button | Action |
|--------|--------|
| **D-Pad ↕** | Select layer |
| **A** | Enter layer (edit nodes) |
| **X** | Cycle blend mode |
| **Y** | Adjust opacity |
| **L** | Toggle mute |
| **R** | Toggle solo |
| **B** | Back |

### Node Editor (Parameter Editing)

| Button | Action |
|--------|--------|
| **D-Pad ↕** | Navigate parameters |
| **D-Pad ←→** | Adjust value (coarse) |
| **A + ←→** | Fine adjust |
| **L / R** | Jump adjust (10× step) |
| **B** | Back to layer editor |

### Performance Mode

| Button | Action |
|--------|--------|
| **D-Pad ↕** | Select FX slot |
| **D-Pad ←→** | Adjust FX intensity |
| **A** | Particle burst |
| **X** | Next scene |
| **Y** | Toggle overlay |
| **B** | Toggle instant/crossfade |
| **L / R** | Crossfade to prev/next scene |
| **Start** | BPM tap |
| **Select** | Exit performance mode |

### Desktop Development

| Key | Maps To |
|-----|---------|
| Arrow keys | D-Pad |
| Z | A |
| X | B |
| A | X (node type) |
| S | Y (preview) |
| Q / W | L / R |
| E / R | L2 / R2 |
| Enter | Start |
| Tab | Select |
| Escape | Quit |

---

## Building from Source

### Desktop (Development)

```bash
# Requires SDL2
# Ubuntu/Debian: sudo apt install libsdl2-dev
# Arch: sudo pacman -S sdl2

make
./pocket-vj
```

### Cross-Compile for MUOS / RG35XX

```bash
# Requires ARM GNU Toolchain
# Download from: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

./scripts/cross-compile.sh
# Binary output: build/pocket-vj
```

See [DEPLOY.md](DEPLOY.md) for detailed deployment instructions.

---

## Creating Visuals — Quick Start

### 1. Add Nodes
Press **X** to open the node browser. Choose a category (Source, FX, 3D, Modulator) and press **A** to add a node to the current layer.

### 2. Edit Parameters
Press **A** to open the layer editor, then **A** again to edit a node's parameters. Use the D-pad to navigate params and adjust values with sliders.

### 3. Stack Layers
Use **L/R** to switch layers. Each layer can have multiple nodes, its own blend mode, and opacity. Layers composite bottom-to-top.

### 4. Apply FX
Add FX nodes (blur, glitch, feedback, scanlines) to process the visuals. FX operate on the framebuffer after source nodes render.

### 5. Go Live
Press **Select+Start** to enter performance mode. Control FX in real-time, switch scenes, tap BPM, trigger particle bursts.

### 6. Save & Share
Press **Select → Save** to save your creation as a `.pvj` preset. Use **Select → Export .pdviz** to create a Pocket DAW visualizer.

---

## Custom Assets

### Particle Sprites
Drop `.bmp` files (max 64×64, 8-bit or 24-bit) into `sprites/particles/`. Use magenta (#FF00FF) for transparency. Select sprites in the particle node's parameter editor.

### 3D Meshes
Drop `.obj` files into `meshes/`. Only vertices and faces are loaded (no textures/materials). Models are auto-centered and normalized. Add a Mesh node and select your file.

---

## Preset Format (.pvj)

Presets are JSON files:
```json
{
  "version": 2,
  "name": "My Visual",
  "bpm": 120.0,
  "nodes": [
    {
      "type": "BARS",
      "active": true,
      "params": {
        "bars": 16,
        "intensity": 1.0,
        "color_r": 255,
        "color_g": 30,
        "color_b": 60
      }
    }
  ]
}
```

---

## Architecture

```
src/
├── main.cpp                    # App loop, state management
├── input.cpp                   # Gamepad + keyboard input
├── config.h                    # Global config, colors, limits
├── preset.cpp                  # JSON preset save/load
├── engine/
│   ├── renderer.cpp            # SDL2 software renderer + bitmap font
│   ├── visual_node.cpp         # Base visual node class
│   ├── param.cpp               # Parameter registry (5 types)
│   ├── layer.cpp               # 8-layer system + blend modes
│   ├── sprite.cpp              # BMP sprite loader + cache
│   ├── scene.cpp               # 8 scene slots + crossfade
│   ├── bpm.cpp                 # BPM tap tempo
│   ├── recorder.cpp            # Frame recording (RLE compressed)
│   ├── nodes/                  # 27 visual node implementations
│   │   ├── bars.cpp, waveform.cpp, shapes.cpp, particles.cpp
│   │   ├── color_field.cpp, noise.cpp, laser.cpp, strobe.cpp
│   │   ├── grid.cpp, tunnel.cpp, starfield.cpp, plasma.cpp
│   │   ├── mirror.cpp, blur.cpp, feedback.cpp, glitch.cpp
│   │   ├── scanline.cpp, pixelate.cpp, colorshift.cpp, edge.cpp
│   │   ├── mesh.cpp, cube.cpp, sphere.cpp, torus.cpp
│   │   └── (each with .h header)
│   ├── audio/
│   │   ├── fft.cpp             # Cooley-Tukey FFT (256-point)
│   │   └── audio_input.cpp     # SDL2 audio capture
│   ├── modulators/
│   │   ├── lfo.cpp             # LFO (sine/tri/saw/square)
│   │   ├── envelope.cpp        # ADSR envelope
│   │   └── audio_mod.cpp       # Audio-reactive modulator
│   └── 3d/
│       ├── math3d.h            # Vec2, Vec3, Mat4
│       ├── rasterizer.cpp      # Triangle rasterizer + Z-buffer
│       ├── obj_loader.cpp      # .OBJ parser + primitive generators
│       └── camera3d.cpp        # Orbit camera
├── sequencer/
│   └── pattern.cpp             # 16-step visual sequencer
├── export/
│   └── pdviz.cpp               # Pocket DAW .pdviz export
└── ui/
    ├── tracker_view.cpp        # Main tracker grid
    ├── preview.cpp             # Visual preview panel
    ├── menu.cpp                # Save/load/export menu
    ├── widgets.cpp             # Slider, knob, toggle widgets
    ├── node_editor.cpp         # Parameter editor
    ├── node_browser.cpp        # Node type browser
    ├── layer_editor.cpp        # Layer management
    ├── file_browser.cpp        # SD card file browser
    └── performance.cpp         # Live performance mode
```

**Stats:** 109 source files, ~8,900 lines of C++, 402KB ARM64 binary

---

## Known Issues

See [TODO.md](TODO.md) for the full list of known issues and planned features.

**Key issues:**
- Performance mode FX slots only work if you've added FX nodes to a layer first
- Preset loading overwrites params but doesn't create new nodes
- Modulator routing UI not yet implemented
- 3D camera settings not accessible from UI

---

## Roadmap

- [x] Phase 1: SDL2 renderer, basic nodes, input
- [x] Phase 2: Parameter system, UI widgets, presets
- [x] Phase 3: Expanded nodes, FFT audio, modulators
- [x] Phase 4: FX pipeline, layers, custom sprites
- [x] Phase 5: Performance mode, scenes, BPM, recording
- [x] Phase 6: Software 3D engine
- [ ] Phase 7: Polish, bug fixes, modulator routing
- [ ] Phase 8: Advanced features (MIDI, video export, networking)

---

## License

MIT

---

*WAValchemy × Pocket VJ — Visual production in your pocket.* ⚡
