#include "recorder.h"
#include <cstdio>
#include <cstring>

Recorder::Recorder() {}
Recorder::~Recorder() {}

void Recorder::startRecording() {
    m_frames.clear();
    m_totalBytes = 0;
    m_recording = true;
}

void Recorder::stopRecording() {
    m_recording = false;
}

void Recorder::captureFrame(const uint32_t* pixels) {
    if (!m_recording) return;
    if ((int)m_frames.size() >= MAX_FRAMES) {
        m_recording = false;  // Auto-stop at max
        return;
    }

    int pixelCount = RENDER_W * RENDER_H;
    auto encoded = rleEncode(pixels, pixelCount);

    // Check memory budget
    if (m_totalBytes + encoded.size() > MAX_MEMORY) {
        m_recording = false;  // Out of budget
        return;
    }

    CompressedFrame frame;
    frame.data = std::move(encoded);
    m_totalBytes += frame.data.size();
    m_frames.push_back(std::move(frame));
}

bool Recorder::saveRecording(const std::string& filename) {
    if (m_frames.empty()) return false;

    FILE* f = fopen(filename.c_str(), "wb");
    if (!f) return false;

    // Simple header: magic + frame count + dimensions
    uint32_t magic = 0x50564A52;  // "PVJR"
    uint32_t count = (uint32_t)m_frames.size();
    uint32_t w = RENDER_W, h = RENDER_H;

    fwrite(&magic, 4, 1, f);
    fwrite(&count, 4, 1, f);
    fwrite(&w, 4, 1, f);
    fwrite(&h, 4, 1, f);

    // Write each frame: size + data
    for (auto& frame : m_frames) {
        uint32_t sz = (uint32_t)frame.data.size();
        fwrite(&sz, 4, 1, f);
        fwrite(frame.data.data(), 1, sz, f);
    }

    fclose(f);
    return true;
}

bool Recorder::playFrame(int index, uint32_t* outPixels) {
    if (index < 0 || index >= (int)m_frames.size()) return false;
    rleDecode(m_frames[index].data, outPixels, RENDER_W * RENDER_H);
    return true;
}

// ---- RLE Compression ----
// Format: [count(1 byte)][R][G][B] — runs of same color up to 255
// For 320x240 with large flat areas, this compresses well

std::vector<uint8_t> Recorder::rleEncode(const uint32_t* pixels, int count) {
    std::vector<uint8_t> out;
    out.reserve(count);  // Worst case won't exceed this much

    int i = 0;
    while (i < count) {
        uint32_t px = pixels[i];
        uint8_t r = (px >> 16) & 0xFF;
        uint8_t g = (px >> 8) & 0xFF;
        uint8_t b = px & 0xFF;

        // Count run
        int run = 1;
        while (i + run < count && run < 255 && pixels[i + run] == px) {
            run++;
        }

        out.push_back((uint8_t)run);
        out.push_back(r);
        out.push_back(g);
        out.push_back(b);

        i += run;
    }

    return out;
}

void Recorder::rleDecode(const std::vector<uint8_t>& data, uint32_t* pixels, int count) {
    int pi = 0;
    size_t di = 0;

    while (di + 3 < data.size() && pi < count) {
        int run = data[di];
        uint8_t r = data[di + 1];
        uint8_t g = data[di + 2];
        uint8_t b = data[di + 3];
        di += 4;

        uint32_t px = 0xFF000000 | (r << 16) | (g << 8) | b;
        for (int j = 0; j < run && pi < count; j++) {
            pixels[pi++] = px;
        }
    }

    // Fill remaining with black if data was short
    while (pi < count) {
        pixels[pi++] = 0xFF000000;
    }
}
