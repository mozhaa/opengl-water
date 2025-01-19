#include "pool.hpp"

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include "stb_image.h"
#include "easylogging++.h"

#include "shaderload.h"

pool::pool(std::string texture_name) {
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

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int width, height, channels;
    std::string image_path = std::string(PROJECT_ROOT) + "/resources/" + texture_name;
    auto pixels = stbi_load(image_path.c_str(), &width, &height, &channels, 4);

    LOG(INFO) << "Image loaded, width=" << width << ", height=" << height << std::endl;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
}

void pool::draw(camera_settings& camera, lighting_settings& lighting) {
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(program, "pool_texture"), 0);

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, reinterpret_cast<float *>(&camera.camera_position));
    
    lighting.set_uniforms(program);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
