#include "envmap.hpp"

#include <string>

#include "stb_image.h"
#include "shaderload.h"

#include "easylogging++.h"

environment_map::environment_map(std::string image_name) {
    program = create_program({
        std::string(SHADERS_DIR) + "/envmap_draw.vert",
        std::string(SHADERS_DIR) + "/envmap_draw.frag",
    });

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int width, height, channels;
    std::string image_path = std::string(PROJECT_ROOT) + "/resources/" + image_name;
    auto pixels = stbi_load(image_path.c_str(), &width, &height, &channels, 4);

    LOG(INFO) << "Image loaded, width=" << width << ", height=" << height << std::endl;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
}

void environment_map::draw(camera_settings& camera) {
    glUseProgram(program);

    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(program, "envmap"), 0);
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, reinterpret_cast<float *>(&camera.camera_position));
    glUniformMatrix4fv(glGetUniformLocation(program, "view_projection_inverse"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view_projection_inverse));

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
