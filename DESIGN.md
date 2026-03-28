# Pocket VJ — Visual Creation System Design

## Philosophy

**No boxes. No instructions. Full control.**

Pocket VJ is a visual synthesizer. Like a music synth gives you oscillators, filters, and envelopes — Pocket VJ gives you visual generators, effects, and modulators. Users build visuals from the ground up.

The UI paradigm: **everything is a parameter, every parameter is tweakable.** Sliders, knobs (rotary values), and numeric inputs — navigated entirely by gamepad.

---

## Core Architecture

### Visual Signal Flow

```
[SOURCE] → [TRANSFORM] → [FX] → [OUTPUT]
   ↑           ↑           ↑
   └── MODULATOR ──────────┘
```

Everything is a **Node**. Nodes have **inputs** and **outputs**. Users wire them together to create visual patches.

### Node Categories

#### 1. SOURCE NODES (Generate visuals)
These create the raw visual material:

| Node | Description | Key Params |
|------|-------------|------------|
| **BARS** | Spectrum analyzer bars | count, width, gap, direction, color, gradient |
| **WAVE** | Oscilloscope waveform | thickness, speed, harmonics, color |
| **SHAPE** | Geometric primitives | type (circle/rect/tri/poly/star), size, rotation, fill, stroke |
| **PARTICLE** | Particle emitter | sprite, count, lifetime, gravity, velocity, spread, burst_trigger |
| **FIELD** | Color fills/gradients | type (solid/gradient_v/gradient_h/radial/pulse), color1, color2 |
| **SPRITE** | Custom image display | file, x, y, scale, rotation, opacity, tint |
| **TEXT** | ASCII/bitmap text | string, font_size, scroll_speed, color |
| **NOISE** | Procedural noise | type (perlin/simplex/worley), scale, speed, octaves |
| **LASER** | Vector line patterns | points, speed, decay, thickness, color |
| **STROBE** | Flash/pulse generator | rate, duty_cycle, color, shape |
| **GRID** | Dot/line grid | rows, cols, spacing, dot_size, wave_amount |
| **TUNNEL** | Infinite tunnel effect | speed, segments, rotation, color_cycle |
| **STARFIELD** | 3D star field | star_count, speed, depth, color |
| **PLASMA** | Classic plasma effect | speed, scale, palette, complexity |

#### 2. TRANSFORM NODES (Modify position/shape)
| Node | Description | Key Params |
|------|-------------|------------|
| **MOVE** | Position offset | x, y, speed, easing |
| **SCALE** | Size transform | x_scale, y_scale, pivot_x, pivot_y |
| **ROTATE** | Rotation | angle, speed, pivot_x, pivot_y |
| **MIRROR** | Symmetry | axis (x/y/both/radial), segments |
| **TILE** | Repeat/tile | cols, rows, spacing, offset |
| **KALEIDOSCOPE** | Kaleidoscope effect | segments, rotation, zoom |

#### 3. FX NODES (Process visuals)
| Node | Description | Key Params |
|------|-------------|------------|
| **BLUR** | Gaussian/box blur | radius, passes |
| **GLOW** | Bloom/glow | threshold, radius, intensity |
| **FEEDBACK** | Frame feedback/trails | decay, offset_x, offset_y, rotation, zoom |
| **INVERT** | Color inversion | amount |
| **COLOR_SHIFT** | Hue/saturation/brightness | hue, saturation, brightness, contrast |
| **PIXELATE** | Mosaic/pixel effect | block_size |
| **SCANLINES** | CRT scanlines | spacing, thickness, intensity |
| **GLITCH** | Digital glitch | intensity, block_size, color_shift |
| **CHROMATIC** | Chromatic aberration | offset, angle |
| **THRESHOLD** | Binary threshold | level, color1, color2 |
| **POSTERIZE** | Reduce color levels | levels |
| **DISTORT** | Wave distortion | amount, speed, frequency |
| **EDGE** | Edge detection | threshold, color |
| **BLEND** | Layer blending | mode (add/multiply/screen/overlay), opacity |

#### 4. MODULATOR NODES (Automate parameters)
| Node | Description | Key Params |
|------|-------------|------------|
| **LFO** | Low-frequency oscillator | shape (sine/tri/saw/square), rate, depth, phase |
| **ENVELOPE** | Attack/decay/sustain/release | A, D, S, R, trigger |
| **RANDOM** | Random value generator | min, max, rate, smoothing |
| **AUDIO_LEVEL** | Audio amplitude follower | band (bass/mid/high/all), smoothing, gain |
| **AUDIO_FFT** | Frequency bin selector | bin, smoothing, gain |
| **BEAT** | Beat detector | sensitivity, decay |
| **STEP** | Step sequencer value | length, values[], speed |
| **RAMP** | Linear ramp | start, end, duration, loop |
| **CLOCK** | BPM-synced clock | division (1/1, 1/2, 1/4...), swing |

#### 5. FUTURE: 3D NODES (Low-poly)
| Node | Description | Key Params |
|------|-------------|------------|
| **MESH** | Load .obj mesh | file, scale, rotation_x/y/z |
| **CUBE** | Primitive cube | size, rotation, wireframe, color |
| **SPHERE** | Primitive sphere | radius, segments, wireframe |
| **PLANE** | Flat plane | width, height, subdivisions |
| **CAMERA** | 3D camera | fov, distance, orbit_speed, look_at |
| **LIGHT** | Point/directional light | type, color, intensity, position |
| **WIREFRAME** | Force wireframe render | line_thickness, color |
| **FLAT_SHADE** | Flat shading | palette, ambient |

---

## UI System

### Screen Layout

```
┌─────────────────────────────────────────────┐
│ [MODE] ─── [NODE NAME] ──── [LAYER 2/8] ───│  ← Status bar (8px)
├─────────────────────────────────────────────┤
│                                             │
│                                             │
│            MAIN CONTENT AREA                │  ← 224px
│         (varies by edit mode)               │
│                                             │
│                                             │
├─────────────────────────────────────────────┤
│ A:EDIT  B:BACK  X:TYPE  Y:PREVIEW  L/R:LYR │  ← Help bar (8px)
└─────────────────────────────────────────────┘
```

### Edit Modes

#### 1. PATCH VIEW (Main screen — see all nodes)
Visual overview of the current patch. Shows all active nodes as blocks with connection lines.

```
┌──────────────────────────────────────┐
│ PATCH ──────────────── Layer 1/4     │
├──────────────────────────────────────┤
│                                      │
│  ┌─FIELD──┐   ┌─BARS───┐            │
│  │ grad_v │──▶│ 16 bar │──┐         │
│  └────────┘   └────────┘  │         │
│                            ▼         │
│  ┌─WAVE───┐   ┌─BLEND──┐  ┌─OUT──┐  │
│  │ scope  │──▶│ screen │─▶│ scr  │  │
│  └────────┘   └────────┘  └──────┘  │
│                                      │
│  ┌─LFO────┐                         │
│  │ sine   │──▶ BARS.color_r          │
│  └────────┘                         │
│                                      │
│ A:EDIT  B:—  X:ADD  Y:PREVIEW       │
└──────────────────────────────────────┘
```

**Controls:**
- D-pad: Navigate between nodes
- A: Enter node editor (parameter view)
- X: Add new node (opens node browser)
- Y: Toggle fullscreen preview
- L/R: Switch layers
- Start: Play/Stop
- Select: Menu (save/load/export)

#### 2. NODE EDITOR (Parameter editing)
When you press A on a node, you enter its parameter editor. THIS is where sliders and knobs live.

```
┌──────────────────────────────────────┐
│ EDIT ── BARS ──────── Layer 1/4      │
├──────────────────────────────────────┤
│                                      │
│  count     ▸ [████████░░░░] 16       │
│  width     ▸ [██████░░░░░░]  8       │
│  gap       ▸ [██░░░░░░░░░░]  2       │
│  direction ▸ ◀ UP ▶                  │
│  color_r   ▸ [██████████░░] 255      │
│  color_g   ▸ [██░░░░░░░░░░]  30     │
│  color_b   ▸ [████░░░░░░░░]  60     │
│  reactive  ▸ [ON]                    │
│  fall_spd  ▸ [██████░░░░░░] 3.0     │
│  gradient  ▸ ◀ NONE ▶               │
│                                      │
│ ← Preview ─────────────── ──────┐   │
│ │ ▓▓▓▓▓ ▓▓▓ ▓▓ ▓▓▓▓ ▓ ▓▓▓ ▓▓ │   │
│ └────────────────────────────────┘   │
│                                      │
│ A:ADJUST  B:BACK  X:MOD  Y:PREVIEW  │
└──────────────────────────────────────┘
```

**Controls:**
- D-pad Up/Down: Navigate parameters
- D-pad Left/Right: Adjust value (coarse)
- A (hold) + D-pad Left/Right: Fine adjust
- X: Attach modulator to selected parameter
- Y: Toggle inline preview
- B: Back to patch view

#### 3. PARAMETER TYPES

**Slider (continuous):**
```
  intensity ▸ [████████░░░░] 0.75
```
- Left/Right: ±0.1 (coarse)
- A+Left/Right: ±0.01 (fine)
- L/R shoulder: ±1.0 (jump)

**Knob (rotary 0-360):**
```
  rotation  ▸ ◉ 127° ────────
```
- Left/Right: ±5° (coarse)
- A+Left/Right: ±1° (fine)

**Integer:**
```
  count     ▸ [████████░░░░] 16
```
- Left/Right: ±1
- L/R: ±10

**Enum (selector):**
```
  direction ▸ ◀ UP ▶
```
- Left/Right: Cycle through options

**Toggle:**
```
  reactive  ▸ [ON]
```
- A or Left/Right: Toggle

**Color (RGB):**
```
  color     ▸ ■ R:255 G:30 B:60
```
- A: Enter color editor (RGB sliders + palette)

**File (asset):**
```
  sprite    ▸ 📁 sparkle.bmp
```
- A: Open file browser

#### 4. NODE BROWSER (Adding new nodes)
```
┌──────────────────────────────────────┐
│ ADD NODE ─────────────────────────── │
├──────────────────────────────────────┤
│                                      │
│  ▸ SOURCE                            │
│    ├─ BARS      Spectrum bars        │
│    ├─ WAVE      Waveform             │
│    ├─ SHAPE     Geometric            │
│    ├─ PARTICLE  Particle emitter     │
│    ├─ SPRITE    Custom image         │
│    ├─ NOISE     Procedural noise     │
│    ├─ LASER     Vector lines         │
│    ├─ TUNNEL    Tunnel effect        │
│    └─ ...                            │
│                                      │
│    TRANSFORM                         │
│    FX                                │
│    MODULATOR                         │
│                                      │
│ A:ADD  B:CANCEL  ↕:BROWSE           │
└──────────────────────────────────────┘
```

#### 5. MODULATOR ROUTING
When pressing X on a parameter, shows available modulators:

```
┌──────────────────────────────────────┐
│ ROUTE ── BARS.color_r ────────────── │
├──────────────────────────────────────┤
│                                      │
│  SOURCE: ◀ LFO ▶                     │
│                                      │
│  shape    ▸ ◀ SINE ▶                 │
│  rate     ▸ [████░░░░░░░░] 2.0 Hz    │
│  depth    ▸ [████████░░░░] 0.8        │
│  phase    ▸ [░░░░░░░░░░░░] 0°        │
│  min      ▸ [░░░░░░░░░░░░] 0         │
│  max      ▸ [████████████] 255       │
│                                      │
│  ── Preview ──────────────────────   │
│  ∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿   │
│                                      │
│ A:CONFIRM  B:CANCEL  X:REMOVE        │
└──────────────────────────────────────┘
```

#### 6. LAYER SYSTEM
8 visual layers, each with:
- Blend mode (normal, add, multiply, screen, overlay)
- Opacity (0-100%)
- Solo / Mute
- Node chain

```
┌──────────────────────────────────────┐
│ LAYERS ──────────────────────────── │
├──────────────────────────────────────┤
│                                      │
│  1 ▸ [████████] BG Field    N 100%   │
│  2 ▸ [████████] Bars+Wave   A  80%   │
│  3 ▸ [████████] Particles   A 100%   │
│  4 ▸ [░░░░░░░░] Shapes      S  60%   │
│  5   [        ] (empty)              │
│  6   [        ] (empty)              │
│  7   [        ] (empty)              │
│  8   [        ] (empty)              │
│                                      │
│ Blend: N=Normal A=Add M=Multiply     │
│        S=Screen O=Overlay            │
│                                      │
│ A:EDIT  X:BLEND  Y:OPACITY  L:MUTE  │
└──────────────────────────────────────┘
```

---

## Custom Assets

### Sprite/Image System

Users can load custom `.bmp` images for:
- Particle sprites (custom particle shapes)
- Background images
- Overlay graphics
- Sprite node display

**Asset folder structure on SD card:**
```
/mnt/mmc/MUOS/application/PocketVJ/
├── pocket-vj
├── mux_launch.sh
├── presets/
│   ├── neon-pulse.pvj
│   └── retro-wave.pvj
├── sprites/
│   ├── particles/        ← Custom particle sprites
│   │   ├── spark.bmp
│   │   ├── star.bmp
│   │   ├── circle.bmp
│   │   └── custom1.bmp
│   ├── backgrounds/      ← Background images
│   └── overlays/         ← Overlay graphics
└── meshes/               ← Future: 3D models (.obj)
    ├── cube.obj
    └── crystal.obj
```

**Sprite constraints (320×240 @ 30fps):**
- Max sprite size: 64×64 pixels (for particles)
- Format: BMP (8-bit indexed or 24-bit RGB)
- Transparency: Color key (magenta #FF00FF = transparent)
- Recommended: 8×8 or 16×16 for particles (performance)

### File Browser
```
┌──────────────────────────────────────┐
│ FILES ── sprites/particles/ ──────── │
├──────────────────────────────────────┤
│                                      │
│  📁 ..                               │
│  ▸ ★ spark.bmp        8×8           │
│    ★ star.bmp         16×16          │
│    ● circle.bmp       8×8           │
│    ◆ diamond.bmp      16×16          │
│    ■ custom1.bmp      32×32          │
│                                      │
│  ┌─ Preview ──────┐                  │
│  │    ★  ★        │                  │
│  │  ★    ★   ★    │                  │
│  │    ★     ★     │                  │
│  └────────────────┘                  │
│                                      │
│ A:SELECT  B:BACK  Y:PREVIEW          │
└──────────────────────────────────────┘
```

---

## Preset System

### Save Format: `.pvj` (JSON)
```json
{
  "version": 2,
  "name": "Neon Pulse",
  "author": "Johnytiger",
  "bpm": 120,
  "layers": [
    {
      "id": 0,
      "blend": "normal",
      "opacity": 1.0,
      "mute": false,
      "nodes": [
        {
          "type": "field",
          "params": {
            "field_type": "gradient_v",
            "color1": [18, 18, 40],
            "color2": [40, 10, 60]
          }
        }
      ]
    },
    {
      "id": 1,
      "blend": "add",
      "opacity": 0.8,
      "nodes": [
        {
          "type": "bars",
          "params": {
            "count": 16,
            "color": [255, 30, 60],
            "reactive": true
          },
          "modulators": {
            "color_r": {
              "type": "lfo",
              "shape": "sine",
              "rate": 0.5,
              "depth": 1.0,
              "min": 60,
              "max": 255
            }
          }
        },
        {
          "type": "wave",
          "params": {
            "color": [60, 255, 180],
            "thickness": 2
          }
        }
      ]
    }
  ],
  "sequence": {
    "length": 16,
    "steps": [
      {"step": 0, "layer": 1, "node": 0, "param": "color_r", "value": 255}
    ]
  },
  "assets": ["sprites/particles/spark.bmp"]
}
```

### Export Format: `.pdviz` (Pocket DAW integration)
Simplified format that Pocket DAW imports as a synth visualizer.

---

## Performance Mode

### Live Control Screen
```
┌──────────────────────────────────────┐
│ LIVE ──── 120 BPM ──── SCENE 1/4 ── │
├──────────────────────────────────────┤
│                                      │
│  ┌──────── VISUAL OUTPUT ─────────┐  │
│  │                                │  │
│  │    (fullscreen preview)        │  │
│  │                                │  │
│  └────────────────────────────────┘  │
│                                      │
│  FX1: GLITCH [░░░░] 0%   ◀────▶     │
│  FX2: BLUR   [██░░] 40%  ◀────▶     │
│  FX3: FEED   [████] 80%  ◀────▶     │
│                                      │
│ ↕:FX  ←→:AMOUNT  A:BURST  X:SCENE   │
└──────────────────────────────────────┘
```

**Controls:**
- D-pad Up/Down: Select FX parameter
- D-pad Left/Right: Adjust FX amount
- A: Trigger particle burst / flash
- X: Switch scene
- Y: Toggle FX overlay visibility
- L/R: Crossfade between scenes
- Start: BPM tap
- Select: Back to edit mode

---

## Future: 3D Pipeline (Low-Poly)

### Software 3D Renderer
Pocket VJ will include a minimal software 3D renderer:
- **Fixed-function pipeline** (no shaders, all CPU)
- **Triangle rasterizer** with Z-buffer
- **Flat shading** (one color per face — classic low-poly look)
- **Wireframe mode** (vector aesthetic)
- **.OBJ loader** (vertices + faces only, no textures initially)
- **Max poly count:** ~500 triangles @ 30fps on Cortex-A53

### 3D Node Workflow
```
[MESH: crystal.obj] → [ROTATE: auto] → [CAMERA: orbit] → [FLAT_SHADE] → [OUTPUT]
                                              ↑
                              [AUDIO_LEVEL: bass] → camera.distance
```

Users load `.obj` files from the `meshes/` folder, apply transforms, and render to the 2D framebuffer. The 3D output composites with 2D layers.

### 3D Primitives (Built-in)
- Cube, Sphere, Torus, Pyramid, Cylinder, Plane
- All generated procedurally (no files needed)

---

## Implementation Phases

### Phase 2: Parameter System (CURRENT)
- [ ] Parameter registry (typed params with min/max/default)
- [ ] Slider/knob/enum/toggle UI widgets
- [ ] Node editor screen with parameter list
- [ ] Real-time inline preview while editing
- [ ] Save/load presets (.pvj JSON)

### Phase 3: Expanded Nodes + Audio
- [ ] All 14 source nodes
- [ ] All 6 transform nodes
- [ ] Real FFT from audio input
- [ ] Beat detection
- [ ] LFO + envelope modulators
- [ ] Modulator routing UI

### Phase 4: FX Pipeline + Assets
- [ ] All 13 FX nodes
- [ ] Layer blend modes
- [ ] Custom sprite loading (BMP)
- [ ] File browser UI
- [ ] Sprite-based particle system

### Phase 5: Performance + DAW Integration
- [ ] Performance mode (live FX control)
- [ ] Scene system (4-8 scenes with crossfade)
- [ ] .pdviz export for Pocket DAW
- [ ] BPM tap + sync

### Phase 6: 3D Engine
- [ ] Software triangle rasterizer
- [ ] Z-buffer
- [ ] .OBJ loader
- [ ] Flat shading + wireframe
- [ ] 3D primitives
- [ ] 3D camera with orbit/fly modes
- [ ] Audio-reactive 3D transforms

---

## Technical Constraints (RG35XX)

| Resource | Limit |
|----------|-------|
| Screen | 320×240 @ 30fps |
| CPU | Cortex-A53 (4 core, 1.5GHz) |
| RAM | ~256MB available |
| GPU | None (software rendering only) |
| Input | Gamepad only (no touch) |
| Storage | SD card (FAT32) |
| Audio | SDL2 audio or ALSA |

### Optimization Strategy
- **Dirty rectangles:** Only redraw changed regions of UI
- **Frame skip:** If rendering takes >33ms, skip visual update but process input
- **LOD for particles:** Reduce particle count if FPS drops
- **Palette mode:** Optional 256-color indexed rendering (faster fills)
- **NEON SIMD:** Use ARM NEON for pixel operations (blur, blend, fill)

---

*Pocket VJ: A visual synthesizer in your pocket.* ⚡
