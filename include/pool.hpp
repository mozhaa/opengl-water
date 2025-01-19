#pragma once

#include <string>

#include <GL/glew.h>

#include "camera.hpp"
#include "lighting_settings.hpp"

struct pool {
    GLuint program, texture, VAO, VBO;

    pool(std::string texture_fp);
    void draw(camera_settings& camera, lighting_settings& lighting);
};
