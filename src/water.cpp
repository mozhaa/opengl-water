#include "water.hpp"

#include <cmath>
#include "easylogging++.h"

#include "shaderload.h"

water::water(int grid_width, int grid_height) : base_height(0.5f), grid_width(grid_width), grid_height(grid_height) {
    program = create_program({
        std::string(SHADERS_DIR) + "/water_draw.vert",
        std::string(SHADERS_DIR) + "/water_draw.frag",
    });

    compute_program = create_program({std::string(SHADERS_DIR) + "/compute_heights.comp"});

    vertices.reserve(grid_width * grid_height);
    indices.reserve((grid_width - 1) * (grid_height - 1) * 2);

    for (int i = 0; i < grid_width; ++i)
        for (int j = 0; j < grid_height; ++j)
            vertices.push_back({(float)i / (grid_width - 1), (float)j / (grid_height - 1)});

    for (int i = 0; i < grid_width - 1; ++i)
        if (i % 2 == 0)
            for (int j = grid_height - 1; j >= 0; --j) {
                indices.push_back((i + 1) * grid_height + j);
                indices.push_back(i * grid_height + j);
            }
        else
            for (int j = 0; j < grid_height; ++j) {
                indices.push_back(i * grid_height + j);
                indices.push_back((i + 1) * grid_height + j);
            }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)(0));

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &heights_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heights_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, grid_width, grid_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, heights_texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void water::update_heights(float time) {
    glUseProgram(compute_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heights_texture);

    glBindImageTexture(0, heights_texture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glUniform1f(glGetUniformLocation(compute_program, "time"), time);

    glDispatchCompute(grid_width, grid_height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void water::draw(camera_settings &camera, lighting_settings &lighting, environment_map& envmap, caustic_drawer& caustic, prepared_heights_texture& heights, int frame_idx) {
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, reinterpret_cast<float *>(&camera.camera_position));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, heights.texture);
    // glBindTexture(GL_TEXTURE_2D, heights_texture);
    glUniform1i(glGetUniformLocation(program, "heights_texture"), 0);
    glUniform1f(glGetUniformLocation(program, "heights_texture_i"), (float)(frame_idx % 100));

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, envmap.texture);
    glUniform1i(glGetUniformLocation(program, "envmap"), 1);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, caustic.caustic_texture);
    glUniform1i(glGetUniformLocation(program, "pool_texture"), 2);

    glUniform3fv(glGetUniformLocation(program, "sun_direction"), 1, reinterpret_cast<float *>(&lighting.sun_direction));
    glUniform3fv(glGetUniformLocation(program, "sun_color"), 1, reinterpret_cast<float *>(&lighting.sun_color));
    glUniform1f(glGetUniformLocation(program, "power"), lighting.power);
    glUniform1f(glGetUniformLocation(program, "specular_strength"), 5.5);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, NULL);

    glCullFace(GL_BACK);
}
