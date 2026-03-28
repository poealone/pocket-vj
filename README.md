# Pocket VJ

**Visual production app for MUOS / RG35XX.** Companion to Pocket DAW.

Create, sequence, and perform visuals on your handheld — then export `.pdviz` presets that Pocket DAW imports as synth visualizers.

## Build (Desktop)

```bash
# Requires SDL2 dev libraries
# Ubuntu/Debian: sudo apt install libsdl2-dev
# Arch: sudo pacman -S sdl2

make
./pocket-vj
```

## Build (MUOS / RG35XX Cross-Compile)

```bash
mkdir build && cd build
cmake .. -DMUOS_BUILD=ON \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/muos-toolchain.cmake
make
```

Copy the `pocket-vj` binary + `presets/` folder to your SD card:
```
/mnt/mmc/MUOS/application/Pocket VJ/
├── pocket-vj
├── launch.sh
└── presets/
```

## Controls

| Key (Desktop) | Button (RG35XX) | Action |
|---|---|---|
| Arrow Keys | D-Pad | Navigate |
| Z | A | Confirm / Trigger |
| X | B | Back / Delete |
| A | X | Toggle node on/off |
| S | Y | Fullscreen preview |
| Q / W | L / R | Switch layer |
| Enter | Start | Play/Stop |
| Tab | Select | Menu |
| Escape | — | Quit (desktop) |

## Visual Nodes

- **FIELD** — Background color fields / gradients
- **BARS** — Spectrum analyzer bars (16-band, audio-reactive)
- **WAVE** — Oscilloscope waveform
- **SHAPE** — Geometric shapes (circle, rect, triangle, line)
- **PTCL** — Particle emitter (gravity, burst, beat-triggered)

## Architecture

```
src/
├── main.cpp              # App loop + state management
├── input.cpp             # Keyboard + gamepad input
├── config.h              # Global config + colors
├── engine/
│   ├── renderer.cpp      # SDL2 software renderer + bitmap font
│   ├── visual_node.cpp   # Base visual node class
│   ├── nodes/            # Visual node implementations
│   └── audio/fft.cpp     # FFT analysis (demo mode, real in Phase 3)
├── sequencer/
│   └── pattern.cpp       # 16-step visual pattern sequencer
└── ui/
    ├── tracker_view.cpp  # Tracker grid UI
    ├── preview.cpp       # Visual preview panel
    └── menu.cpp          # Save/load/export menu
```

## Roadmap

- [x] **Phase 1:** SDL2 renderer + visual nodes + input handling
- [ ] **Phase 2:** 16-step sequencer + pattern save/load
- [ ] **Phase 3:** FX pipeline + FFT audio + beat detection
- [ ] **Phase 4:** `.pdviz` export → Pocket DAW integration
- [ ] **Phase 5:** Performance mode (live FX, scenes, recording)

---

*WAValchemy × Pocket VJ — Visual production in your pocket.* ⚡
