#pragma once

#include <vector>

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include "camera.hpp"
#include "lighting_settings.hpp"
#include "envmap.hpp"
#include "caustic.hpp"
#include "heights_texture.hpp"

struct water {
    GLuint program, VAO, VBO, EBO, compute_program, heights_texture;

    std::vector<glm::vec2> vertices;
    std::vector<uint32_t> indices;

    float base_height;
    int grid_width, grid_height;

    water(int grid_width, int grid_height);
    void update_heights(float time);
    void draw(camera_settings& camera, lighting_settings& lighting, environment_map& envmap, caustic_drawer& caustic, prepared_heights_texture& heights, int frame_idx);
};