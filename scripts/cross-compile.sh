#!/bin/bash
# Cross-compile Pocket VJ for MUOS / RG35XX (ARM64)
# Usage: ./scripts/cross-compile.sh

set -e

echo "🔨 Cross-compiling Pocket VJ for MUOS/RG35XX..."

# Check for cross-compiler
if ! command -v aarch64-linux-gnu-g++ &> /dev/null; then
    echo "❌ ARM64 cross-compiler not found!"
    echo "   Install on Debian/Ubuntu:"
    echo "   sudo apt-get install -y crossbuild-essential-arm64 libsdl2-dev:arm64 cmake"
    exit 1
fi

echo "✅ Found aarch64-linux-gnu-g++"

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "🛠️  Configuring CMake..."
cmake .. \
    -DMUOS_BUILD=ON \
    -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_FIND_ROOT_PATH=/usr/aarch64-linux-gnu

# Build
echo "🚀 Building..."
make -j$(nproc)

# Output
BINARY=pocket-vj
if [ -f "$BINARY" ]; then
    echo "✅ Build successful!"
    echo "📦 Binary: build/$BINARY"
    file "$BINARY"
    echo ""
    echo "📋 Next steps:"
    echo "   1. Copy to SD card:"
    echo "      mkdir -p /mnt/sd-card/MUOS/application/Pocket\\ VJ/"
    echo "      cp build/pocket-vj /mnt/sd-card/MUOS/application/Pocket\\ VJ/"
    echo "      cp -r presets/ /mnt/sd-card/MUOS/application/Pocket\\ VJ/"
    echo "      cp launch.sh /mnt/sd-card/MUOS/application/Pocket\\ VJ/"
    echo ""
    echo "   2. Eject SD card and insert into RG35XX"
    echo "   3. Launch from MUOS app menu"
else
    echo "❌ Build failed!"
    exit 1
fi
