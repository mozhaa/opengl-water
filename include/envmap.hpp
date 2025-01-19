#pragma once

#include <string>

#include <GL/glew.h>

#include "camera.hpp"

struct environment_map {
    GLuint texture, program;

    environment_map(std::string image_name);
    void draw(camera_settings& camera);
};
