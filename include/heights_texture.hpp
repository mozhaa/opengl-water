#pragma once

#include <string>

#include <GL/glew.h>

struct prepared_heights_texture {
    GLuint texture;

    prepared_heights_texture(std::string directory);
};