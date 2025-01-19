#include "pool.hpp"

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include "stb_image.h"
#include "easylogging++.h"

#include "shaderload.h"

pool::pool() {
    program = create_program({
        std::string(SHADERS_DIR) + "/pool_draw.vert",
        std::string(SHADERS_DIR) + "/pool_draw.frag",
    });

    std::vector<std::pair<glm::vec3, glm::vec2>> vertices = {
        {{-1.f, 0.f, -1.f}, {0.f, 0.f}},
        {{-1.f, 0.f, 1.f}, {0.f, 1.f}},
        {{1.f, 0.f, -1.f}, {1.f, 0.f}},
        {{1.f, 0.f, 1.f}, {1.f, 1.f}},
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type), (void*)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type), (void*)(sizeof(glm::vec3)));
}

void pool::draw(camera_settings& camera, lighting_settings& lighting, caustic_drawer& caustic) {
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, caustic.caustic_texture);

    glUniform1i(glGetUniformLocation(program, "pool_texture"), 0);

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, reinterpret_cast<float *>(&camera.camera_position));
    
    lighting.set_uniforms(program);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
