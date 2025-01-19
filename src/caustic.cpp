#include "caustic.hpp"

#include <stdexcept>

#include "stb_image.h"
#include "easylogging++.h"

#include "shaderload.h"

caustic_drawer::caustic_drawer(std::string texture_name) {
    program = create_program({
        std::string(SHADERS_DIR) + "/caustic_draw.vert",
        std::string(SHADERS_DIR) + "/caustic_draw.frag",
    });
    
    copy_program = create_program({
        std::string(SHADERS_DIR) + "/copy_texture.vert",
        std::string(SHADERS_DIR) + "/copy_texture.frag",
    });

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &pool_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pool_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int channels;
    std::string image_path = std::string(PROJECT_ROOT) + "/resources/" + texture_name;
    auto pixels = stbi_load(image_path.c_str(), &width, &height, &channels, 4);

    LOG(INFO) << "Image loaded, width=" << width << ", height=" << height << std::endl;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &caustic_texture);
    glBindTexture(GL_TEXTURE_2D, caustic_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, caustic_texture, 0);
    GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to initialize framebuffer for caustics");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void caustic_drawer::update(GLuint water_VAO, std::vector<uint32_t>& water_indices, GLuint heights_texture, glm::vec3 sun_direction) {
    // glCopyImageSubData(P.texture, GL_TEXTURE_2D, 0, 0, 0, 0,
    //                    texture, GL_TEXTURE_2D, 0, 0, 0, 0,
    //                    width, height, 1);
    glUseProgram(copy_program);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, width, height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pool_texture);

    glUniform1i(glGetUniformLocation(copy_program, "src"), 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(program);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heights_texture);
    glUniform1i(glGetUniformLocation(program, "heights_texture"), 0);

    glUniform3fv(glGetUniformLocation(program, "sun_direction"), 1, reinterpret_cast<float *>(&sun_direction));

    glBindVertexArray(water_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, water_indices.size(), GL_UNSIGNED_INT, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
}