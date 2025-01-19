#include "recording.hpp"

#include <ctime>
#include <sstream>
#include <iomanip>

#include <GL/glew.h>
#include "easylogging++.h"

bool recorder::is_recording() const {
    return ffmpeg;
}

void recorder::start_recording(int width, int height) {
    this->width = width;
    this->height = height;
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream output_fp;
    output_fp << "\"" << PROJECT_ROOT << "/resources/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ".mp4\"";

    std::string command = (
        std::string("ffmpeg -y -f rawvideo -video_size ") + 
        std::to_string(width) + "x" + std::to_string(height) + 
        " -pix_fmt rgb24 -r " + std::to_string(fps) + 
        " -i - -vf vflip -an -c:v libx264 " + output_fp.str()
    );
    ffmpeg = popen(command.c_str(), "w");
    if (!ffmpeg)
        throw std::runtime_error("Failed to start ffmpeg process.");
    pixels.reserve(width * height * 3);
    LOG(INFO) << "ffmpeg recording started, output=" << output_fp.str();
}

void recorder::stop_recording() {
    pclose(ffmpeg);
    ffmpeg = NULL;
    LOG(INFO) << "ffmpeg recording stopped";
}

void recorder::save_frame() {
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    fwrite(pixels.data(), width * height * 3, 1, ffmpeg);
}

recorder::recorder(float fps) : fps(fps), ffmpeg(NULL) {}
