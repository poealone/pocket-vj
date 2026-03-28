#!/bin/sh
# Run inside Docker Alpine container
set -e

apk add --no-cache wget make tar sdl2-dev > /dev/null 2>&1
echo "==> Packages installed"

echo "==> Downloading aarch64 cross-compiler..."
wget -q 'https://musl.cc/aarch64-linux-musl-cross.tgz' -O /tmp/toolchain.tgz
tar xzf /tmp/toolchain.tgz -C /opt/
CXX=/opt/aarch64-linux-musl-cross/bin/aarch64-linux-musl-g++

echo "==> Compiler: $($CXX --version | head -1)"

cd /src

# Clean old artifacts
find . -name '*.arm64.o' -delete 2>/dev/null
rm -f pocket-vj-arm64

CFLAGS="-std=c++17 -O2 -I/usr/include/SDL2 -Isrc -DSCREEN_W=320 -DSCREEN_H=240 -DTARGET_FPS=30 -DMUOS_BUILD"

SRCS="src/main.cpp src/input.cpp
src/engine/renderer.cpp src/engine/visual_node.cpp
src/engine/nodes/bars.cpp src/engine/nodes/waveform.cpp
src/engine/nodes/shapes.cpp src/engine/nodes/particles.cpp
src/engine/nodes/color_field.cpp
src/engine/audio/fft.cpp
src/sequencer/pattern.cpp
src/ui/tracker_view.cpp src/ui/preview.cpp src/ui/menu.cpp"

echo "==> Compiling..."
OBJS=""
for f in $SRCS; do
    OUT="${f%.cpp}.arm64.o"
    echo "  CC $f"
    $CXX $CFLAGS -c "$f" -o "$OUT"
    OBJS="$OBJS $OUT"
done

echo "==> Linking..."
# Link without SDL2 lib (device provides it), just resolve symbols we need
$CXX -o pocket-vj-arm64 $OBJS -Wl,--as-needed 2>&1 || {
    echo "==> Minimal link failed, trying with -lSDL2 stub..."
    # Create a stub SDL2 library for linking
    echo "" | $CXX -x c - -shared -o /tmp/libSDL2.so -Wl,-soname,libSDL2-2.0.so.0 2>/dev/null || true
    $CXX -o pocket-vj-arm64 $OBJS -L/tmp -lSDL2 -Wl,-rpath,/usr/lib 2>&1 || {
        echo "BUILD FAILED"
        exit 1
    }
}

ls -la pocket-vj-arm64
echo "==> BUILD SUCCESS"
