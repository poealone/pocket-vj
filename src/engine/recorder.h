#pragma once
#include "renderer.h"
#include "../config.h"
#include <vector>
#include <string>
#include <cstdint>

// RLE-compressed frame data
struct CompressedFrame {
    std::vector<uint8_t> data;  // RLE encoded
};

// Records visual output frames with RLE compression
class Recorder {
public:
    Recorder();
    ~Recorder();

    void startRecording();
    void stopRecording();
    bool isRecording() const { return m_recording; }

    // Capture current frame from renderer pixels
    void captureFrame(const uint32_t* pixels);

    // Save recording to file
    bool saveRecording(const std::string& filename);

    // Playback: get frame data (decompressed to pixel buffer)
    bool playFrame(int index, uint32_t* outPixels);

    int frameCount() const { return (int)m_frames.size(); }
    int maxFrames() const { return MAX_FRAMES; }

    // Memory usage estimate
    size_t memoryUsed() const { return m_totalBytes; }

    static constexpr int MAX_FRAMES = 300;       // 10 seconds at 30fps
    static constexpr size_t MAX_MEMORY = 2 * 1024 * 1024;  // 2MB budget

private:
    bool m_recording = false;
    std::vector<CompressedFrame> m_frames;
    size_t m_totalBytes = 0;

    // RLE encode/decode
    std::vector<uint8_t> rleEncode(const uint32_t* pixels, int count);
    void rleDecode(const std::vector<uint8_t>& data, uint32_t* pixels, int count);
};
