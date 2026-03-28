# Pocket VJ — Known Issues & TODO

## 🐛 Bugs / Not Working

### Performance Mode FX
- **FX slots auto-assign** but only find FX nodes that already exist in layers. If you haven't added any FX nodes (blur, glitch, feedback, etc.), the 3 FX slots show "---" and nothing happens.
- **Fix needed:** Allow user to assign FX nodes to slots (or auto-create 3 default FX when entering performance mode)
- **Workaround:** Before entering performance mode, add FX nodes (X → FX category → add blur/glitch/feedback) to a layer

### Preset Loading
- **Loading presets only overwrites params on existing nodes** — it doesn't create new nodes or change node types. If you load a preset with 6 nodes but only have 5, the 6th node is ignored.
- **Fix needed:** Preset load should clear all layers and recreate nodes from the preset data

### Tracker View
- **Tracker view shows the step sequencer** but the sequencer currently only automates params on the flat node list by index — doesn't account for layer structure
- **Fix needed:** Sequencer should reference nodes by layer + index, not flat index

### Modulator Routing
- **Modulators (LFO, Envelope, Audio) exist as nodes** but there's no UI to route a modulator's output to another node's parameter
- **Fix needed:** Add modulator routing screen (X on a param in node editor → pick modulator → pick output range)

### Node Editor Access
- **From tracker view, pressing A opens layer editor** → then A again enters first node. But you can't easily navigate between nodes within a layer.
- **Fix needed:** Add Left/Right navigation between nodes within a layer in the node editor

### 3D Camera
- **Camera is shared across all 3D nodes** but there's no UI to adjust camera parameters (FOV, distance, orbit speed)
- **Fix needed:** Add camera settings accessible from menu or as a special node

### Exit Button
- **Menu EXIT is at index 8** but only 7 items are visible — EXIT may be off-screen
- **Fix needed:** Fix menu item indexing

## 🔧 Missing Features (Planned)

### UI Polish
- [ ] Cursor/selection highlight needs more visual contrast
- [ ] Toast/notification system for save/load/export confirmation
- [ ] Undo/redo for param changes
- [ ] Copy/paste nodes between layers
- [ ] Node reorder within a layer (move up/down)
- [ ] Delete node from layer (currently can only toggle active)

### Preset System
- [ ] Preset preview (thumbnail or description)
- [ ] Preset categories/folders
- [ ] Factory presets vs user presets distinction
- [ ] Import/share presets via SD card

### Audio
- [ ] Audio input device selection (if multiple devices)
- [ ] Audio gain/sensitivity adjustment in settings
- [ ] Visual indicator showing audio input level

### Visual Nodes
- [ ] Text/ASCII node (scrolling text overlays)
- [ ] Image/background node (load full BMP as background)
- [ ] Gradient node with more than 2 colors
- [ ] Sprite animation (multi-frame sprite sheets)

### 3D Engine
- [ ] Texture mapping on 3D meshes
- [ ] Multiple light sources
- [ ] Gouraud shading (smooth shading)
- [ ] More built-in meshes (cylinder, cone, icosphere)
- [ ] 3D particle system

### Performance Mode
- [ ] User-assignable FX slots (pick which FX to control)
- [ ] MIDI input support (if hardware supports it)
- [ ] Auto-scene switching (timed or beat-triggered)
- [ ] Visual transition effects between scenes (wipe, dissolve)

### Export
- [ ] GIF export (animated)
- [ ] Video export (raw frames → ffmpeg)
- [ ] Share presets as QR code

### Settings
- [ ] Settings screen (brightness, frame rate target, audio source)
- [ ] Key mapping configuration
- [ ] Theme/color scheme for UI

## 📋 Priority for Tomorrow

1. **Fix Performance Mode FX** — auto-create 3 FX nodes or let user assign
2. **Fix Preset Loading** — clear + recreate nodes from preset data
3. **Fix Menu EXIT** — ensure all items visible and EXIT works
4. **Add node deletion** — B in node editor to delete node
5. **Add node navigation** — Left/Right in node editor to switch nodes in layer
6. **Add modulator routing UI** — the whole point of modulators
7. **Add camera controls** — settings for 3D camera
8. **Polish UI** — better visual feedback, confirmations

---

*Last updated: March 28, 2026*
