# Pocket VJ — Visual Production App for RG35XX

## Overview
A tracker-style visual production app for the RG35XX handheld (GarlicOS). The companion to **Pocket DAW** — where Pocket DAW makes the music, Pocket VJ makes the visuals.

**Goal:** Create, sequence, and perform visuals on the handheld, then export them as `.pdviz` presets that Pocket DAW imports as synth/track visualizers.

---

## 🔗 Pocket DAW Integration (Critical)

### Visual Format: `.pdviz`
The Pocket DAW already supports `.pdviz` files as custom visualizer presets loaded by plugins via `manifest.json`:

```json
"viz": {
  "type": "waveform",
  "x": 6, "y": 14, "w": 300, "h": 32,
  "color": [255, 30, 60],
  "reactive": true,
  "bars": 16
}
```

**Three visual layers in Pocket DAW:**
1. **Waveform** — `pdsynth_get_waveform()` (position/size configurable)
2. **Spectrum Bars** — 4-32 bars, custom color, audio-reactive
3. **Custom Viz** — `.pdviz` preset file (background shader layer)

### Export Pipeline: VJ → DAW
- Pocket VJ exports `.pdviz` files to `/Visuals/` on SD card
- Pocket DAW reads `.pdviz` from plugin folders or `/Visuals/`
- Visual reacts to the synth's own audio channel (per-track isolation)
- Eventually: real-time VJ ↔ DAW sync via shared memory or IPC

---

## 🎨 Core Features

### Visual Engine
- **Resolution:** 320×240 (RG35XX native)
- **Frame rate:** 30fps target (ARM CPU constraints)
- **Color depth:** 16-bit RGB565 (fast rendering)
- **Render pipeline:** SDL2 software renderer (no GPU)

### Visual Types (Nodes)
1. **Waveform** — oscilloscope-style line drawing
2. **Spectrum Bars** — FFT frequency bands (4-32 bars)
3. **Particle Systems** — simple emitters (gravity, velocity, lifetime)
4. **Geometric Shapes** — circles, rectangles, lines, polygons
5. **Text/Glyphs** — terminal-style text overlays (ASCII art)
6. **Color Fields** — gradient backgrounds, solid fills
7. **Noise/Glitch** — procedural noise, scanlines, CRT effects
8. **Pixel Sprites** — small bitmap sprites (8x8, 16x16)

### Sequencer (Tracker-Style)
Like Pocket DAW's step sequencer but for visual events:

```
STEP | NODE     | PARAM    | VALUE
  00 | BARS     | COLOR    | FF1E3C
  01 | ---      | ---      | ---
  02 | PARTICLE | EMIT     | BURST
  03 | GLITCH   | INTENSE  | 80
  04 | BARS     | COLOR    | 3C1EFF
```

- 16/32/64 step patterns
- BPM-synced (matches Pocket DAW tempo)
- Multiple visual tracks (layers)
- Parameter automation per step

### FX Pipeline
- **Color shift** — hue rotation, saturation, brightness
- **Blur** — simple box blur (light on CPU)
- **Invert** — color inversion
- **Feedback** — frame buffer feedback (trails)
- **Scanlines** — CRT/VHS aesthetic
- **Pixelate** — downscale + upscale
- **Mirror** — horizontal/vertical flip

### Audio Input (for reactive visuals)
- Read audio from Pocket DAW via shared buffer or line-in
- FFT analysis (8-band minimum)
- Beat detection (kick/snare triggers)
- Amplitude envelope follower

---

## 🎮 Controls Mapping

| Button | Action |
|--------|--------|
| D-Pad | Navigate grid / move cursor |
| A | Place visual event / confirm |
| B | Delete event / back |
| X | Change node type |
| Y | Change parameter |
| L | Previous pattern / layer |
| R | Next pattern / layer |
| L2 | Decrease value |
| R2 | Increase value |
| Start | Play/Stop sequence |
| Select | Menu (save/load/export) |

### Performance Mode (hold Select + Start)
- D-Pad controls live FX intensity
- A/B/X/Y trigger visual presets
- L/R crossfade between scenes

---

## 📁 File Structure

```
pocket-vj/
├── main.cpp
├── CMakeLists.txt
├── src/
│   ├── engine/
│   │   ├── renderer.cpp      # SDL2 rendering pipeline
│   │   ├── renderer.h
│   │   ├── visual_node.cpp   # Base visual node class
│   │   ├── visual_node.h
│   │   ├── nodes/
│   │   │   ├── waveform.cpp
│   │   │   ├── bars.cpp
│   │   │   ├── particles.cpp
│   │   │   ├── shapes.cpp
│   │   │   ├── text.cpp
│   │   │   ├── noise.cpp
│   │   │   └── sprites.cpp
│   │   ├── fx/
│   │   │   ├── color_shift.cpp
│   │   │   ├── blur.cpp
│   │   │   ├── feedback.cpp
│   │   │   ├── scanlines.cpp
│   │   │   └── pixelate.cpp
│   │   └── audio/
│   │       ├── fft.cpp        # FFT analysis
│   │       ├── beat_detect.cpp
│   │       └── audio_input.cpp
│   ├── sequencer/
│   │   ├── pattern.cpp        # Visual pattern data
│   │   ├── timeline.cpp       # Pattern arrangement
│   │   └── automation.cpp     # Parameter curves
│   ├── ui/
│   │   ├── tracker_view.cpp   # Main tracker grid
│   │   ├── preview.cpp        # Live visual preview
│   │   ├── node_editor.cpp    # Node parameter editing
│   │   └── menu.cpp           # Save/load/export menus
│   └── export/
│       ├── pdviz_export.cpp   # Export to .pdviz format
│       └── gif_export.cpp     # Export preview as GIF (optional)
├── presets/
│   ├── default.pdviz
│   ├── retro-bars.pdviz
│   ├── glitch-storm.pdviz
│   └── neon-wave.pdviz
├── sprites/
│   ├── wavalchemy-logo.bmp
│   └── icons.bmp
└── build/
```

---

## 🔧 Tech Stack

- **Language:** C++ (matches Pocket DAW)
- **Graphics:** SDL2 (software renderer)
- **Audio analysis:** Custom FFT (lightweight, no FFTW dependency)
- **Target:** GarlicOS / ARM Linux (RG35XX)
- **Build:** CMake cross-compilation (same toolchain as Pocket DAW)

---

## 📐 .pdviz Format Specification

```json
{
  "version": 1,
  "name": "Neon Pulse",
  "author": "WAValchemy",
  "bpm_sync": true,
  "resolution": [320, 240],
  "layers": [
    {
      "type": "bars",
      "bars": 16,
      "color": [255, 30, 60],
      "reactive": true,
      "position": { "x": 0, "y": 180, "w": 320, "h": 60 }
    },
    {
      "type": "waveform",
      "color": [60, 255, 180],
      "thickness": 2,
      "position": { "x": 0, "y": 80, "w": 320, "h": 80 }
    },
    {
      "type": "particles",
      "count": 32,
      "color": [255, 255, 255],
      "gravity": -0.5,
      "lifetime": 60,
      "trigger": "beat"
    }
  ],
  "fx": [
    { "type": "scanlines", "intensity": 0.3 },
    { "type": "feedback", "decay": 0.85 }
  ],
  "sequence": [
    { "step": 0, "layer": 0, "param": "color", "value": [255, 30, 60] },
    { "step": 8, "layer": 0, "param": "color", "value": [60, 30, 255] }
  ]
}
```

---

## 🚀 Development Roadmap

### Phase 1: Foundation
- [ ] SDL2 renderer at 320×240 @ 30fps
- [ ] Basic visual nodes (bars, waveform, shapes)
- [ ] Render to framebuffer
- [ ] Button input handling

### Phase 2: Sequencer
- [ ] 16-step visual pattern grid
- [ ] Parameter automation per step
- [ ] BPM clock (internal)
- [ ] Pattern save/load

### Phase 3: FX & Audio
- [ ] FX pipeline (scanlines, feedback, color shift)
- [ ] FFT audio analysis
- [ ] Beat detection
- [ ] Audio-reactive node parameters

### Phase 4: DAW Integration
- [ ] .pdviz export (Pocket DAW compatible)
- [ ] Shared BPM sync
- [ ] Per-track visual assignment
- [ ] Real-time VJ ↔ DAW communication

### Phase 5: Performance Mode
- [ ] Live FX control via buttons
- [ ] Scene crossfading
- [ ] Preset triggering
- [ ] Recording visual performances

---

## ⚠️ Constraints (RG35XX Hardware)
- **CPU:** ARM ~1 GHz (no GPU acceleration)
- **RAM:** ~256-512 MB
- **Display:** 320×240 (IPS, 60Hz panel but we target 30fps)
- **Storage:** SD card (presets loaded on demand)
- **Input:** D-Pad + 8 face buttons + 2 shoulder buttons
- **No touchscreen** — all UI is button-driven

---

*WAValchemy × Pocket VJ — Visual production in your pocket.*
