#pragma once

#include <string>

#include <GL/glew.h>

#include "camera.hpp"
#include "lighting_settings.hpp"
#include "caustic.hpp"

struct pool {
    GLuint program, VAO, VBO;
    int width, height;

    pool();
    void draw(camera_settings& camera, lighting_settings& lighting, caustic_drawer& caustic);
};
