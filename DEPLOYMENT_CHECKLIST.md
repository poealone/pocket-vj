# Pocket VJ — Deployment Checklist

## ✅ Phase 1 Complete

- [x] SDL2 software renderer (320×240, 30fps on device)
- [x] 5 visual nodes (bars, waveform, shapes, particles, color field)
- [x] 16-step pattern sequencer with BPM sync
- [x] Tracker UI + preview panel + menu overlay
- [x] Full RG35XX gamepad mapping
- [x] FFT analyzer stub with demo mode
- [x] MUOS launch script
- [x] GitHub repo setup
- [x] Deployment guide

## 🚀 To Deploy to Device

### Method 1: Cross-Compile (Recommended)

**On your Linux machine:**
```bash
git clone https://github.com/poealone/pocket-vj.git
cd pocket-vj
./scripts/cross-compile.sh
```

Then copy the binary + files to SD card:
```bash
mkdir -p /mnt/sd-card/MUOS/application/Pocket\ VJ/
cp build/pocket-vj /mnt/sd-card/MUOS/application/Pocket\ VJ/
cp -r presets/ /mnt/sd-card/MUOS/application/Pocket\ VJ/
cp launch.sh /mnt/sd-card/MUOS/application/Pocket\ VJ/
```

### Method 2: Wait for Pre-Built Release
We can build ARM64 binaries and create a GitHub Release with:
- `pocket-vj` (compiled for RG35XX)
- `presets/` folder
- `launch.sh`
- Ready to drop on SD card

## 📱 After Deploying to Device

1. Insert SD card into RG35XX
2. Boot MUOS
3. Open app menu
4. Find "Pocket VJ" in the applications list
5. Launch

## 🎮 Controls

| Desktop | Device | Action |
|---------|--------|--------|
| Arrow keys | D-Pad | Navigate |
| Z | A | Confirm / Trigger |
| X | B | Back |
| A | X | Toggle node on/off |
| S | Y | Fullscreen preview |
| Q/W | L/R | Switch layer |
| Enter | Start | Play/Stop |
| Tab | Select | Menu |

## 🛠️ Next Steps (Phase 2)

- [ ] Pattern save/load
- [ ] Multiple pattern storage
- [ ] Preset import/export
- [ ] More visual nodes (glitch, text, etc.)

---

## 📊 Stats

- **Lines of Code:** ~2,300
- **Binary Size:** 55KB (debug build)
- **Build Time:** ~5 sec (desktop)
- **Memory:** ~10MB runtime
- **Framework:** C++ + SDL2
- **Target FPS:** 30 (device), 60 (desktop)
- **Resolution:** 320×240 (native)

---

**Repo:** https://github.com/poealone/pocket-vj
**Issues/Features:** GitHub Issues
