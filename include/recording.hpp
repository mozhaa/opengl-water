#pragma once

#include <cstdio>
#include <cstdint>
#include <vector>

struct recorder {
    FILE *ffmpeg = NULL;
    float fps = 60.f;
    std::vector<uint32_t> pixels;
    int width;
    int height;

    bool is_recording() const;
    void start_recording(int width, int height);
    void stop_recording();
    void save_frame();
    recorder(float fps = 60.f);
};