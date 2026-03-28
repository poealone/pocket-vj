# Pocket VJ — Deployment Guide

## Quick Start (MUOS / RG35XX)

### Option 1: Pre-Built Binary (Easiest)
If you have a pre-built ARM64 binary:

1. Copy the binary + presets to your SD card:
   ```bash
   mkdir -p /mnt/mmc/MUOS/application/Pocket\ VJ/
   cp pocket-vj /mnt/mmc/MUOS/application/Pocket\ VJ/
   cp -r presets/ /mnt/mmc/MUOS/application/Pocket\ VJ/
   cp launch.sh /mnt/mmc/MUOS/application/Pocket\ VJ/
   chmod +x /mnt/mmc/MUOS/application/Pocket\ VJ/launch.sh
   ```

2. Eject SD card, insert into RG35XX
3. Launch from MUOS app menu

### Option 2: Build from Source (Cross-Compile)

#### On Linux (macOS / Windows needs WSL2)

**Prerequisites:**
- ARM64 cross-compiler (`aarch64-linux-gnu-g++`)
- CMake
- SDL2 development headers for ARM64

**Install cross-compiler (Debian/Ubuntu):**
```bash
sudo apt-get update
sudo apt-get install -y \
  crossbuild-essential-arm64 \
  libsdl2-dev:arm64 \
  cmake
```

**Build:**
```bash
mkdir build && cd build
cmake .. \
  -DMUOS_BUILD=ON \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
make
```

**Deploy:**
```bash
mkdir -p /mnt/sd-card/MUOS/application/Pocket\ VJ/
cp build/pocket-vj /mnt/sd-card/MUOS/application/Pocket\ VJ/
cp -r presets/ /mnt/sd-card/MUOS/application/Pocket\ VJ/
cp launch.sh /mnt/sd-card/MUOS/application/Pocket\ VJ/
chmod +x /mnt/sd-card/MUOS/application/Pocket\ VJ/launch.sh
```

#### Using Docker (if you don't have cross-compiler)

```bash
docker run -it --rm \
  -v $(pwd):/src \
  debian:bookworm bash

# Inside container:
dpkg --add-architecture arm64
apt-get update
apt-get install -y crossbuild-essential-arm64 libsdl2-dev:arm64 cmake

cd /src
mkdir build && cd build
cmake .. -DMUOS_BUILD=ON \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
make
```

Then copy the binary as above.

---

## Directory Structure on SD Card

```
/mnt/mmc/MUOS/
└── application/
    └── Pocket VJ/
        ├── pocket-vj          # Main executable
        ├── launch.sh          # Launcher script
        └── presets/
            └── neon-pulse.pdviz
```

The app will look for presets in the same directory as the binary.

---

## Troubleshooting

**App doesn't appear in MUOS menu:**
- Check folder name (must be exactly `Pocket VJ`)
- Verify `launch.sh` is executable: `chmod +x launch.sh`
- Check SD card mount point (usually `/mnt/mmc` on MUOS)

**Binary doesn't run:**
- Verify ARM64 architecture: `file pocket-vj` → should say `ELF 64-bit ARM`
- Check dependencies: `aarch64-linux-gnu-objdump -p pocket-vj | grep NEEDED`
- MUOS should have SDL2 pre-installed; if not, copy SDL2 libs to app folder

**Controls not responding:**
- Ensure `launch.sh` sets `SDL_VIDEODRIVER=fbcon` and `SDL_NOMOUSE=1`
- Test on desktop first: `./pocket-vj` (with X11 or Wayland available)

---

## Development

For desktop development:
```bash
make
./pocket-vj
```

Controls:
- **Arrow keys** / **D-Pad**: Navigate
- **Z** / **A**: Confirm
- **X** / **B**: Back
- **A** / **X**: Toggle node
- **S** / **Y**: Fullscreen preview
- **Q/W** / **L/R**: Switch layer
- **Enter** / **Start**: Play/Stop
- **Tab** / **Select**: Menu
- **Esc**: Quit (desktop only)

---

## Contributing

1. Fork the repo
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Commit your changes
4. Push and open a pull request

**Roadmap:**
- Phase 2: Sequencer save/load, pattern management
- Phase 3: FFT audio analysis, beat detection, real FX
- Phase 4: `.pdviz` export, Pocket DAW integration
- Phase 5: Performance mode, live FX control

---

*WAValchemy × Pocket VJ* ⚡
