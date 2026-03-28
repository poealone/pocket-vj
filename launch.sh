#!/bin/sh
# MUOS Application Launcher for Pocket VJ
# Place this folder in: /mnt/mmc/MUOS/application/Pocket VJ/
# Or SD card: /APPS/PocketVJ/

APPDIR=$(dirname "$0")
cd "$APPDIR"

# Set display for MUOS framebuffer
export SDL_VIDEODRIVER=fbcon
export SDL_FBDEV=/dev/fb0
export SDL_NOMOUSE=1

# Launch
exec ./pocket-vj
