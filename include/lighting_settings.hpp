#pragma once

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <GL/glew.h>

struct lighting_settings {
    glm::vec3 ambient_strength;
    glm::vec3 diffuse_strength;
    glm::vec3 specular_strength;

    glm::vec3 sun_direction;
    glm::vec3 sun_color;

    float power;

    void set_uniforms(GLuint program);
};