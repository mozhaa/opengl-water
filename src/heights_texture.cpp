#include "heights_texture.hpp"

#include <fstream>
#include <vector>

prepared_heights_texture::prepared_heights_texture(std::string file) {
    glEnable(GL_TEXTURE_2D_ARRAY);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    const int width = 512, height = 512, layers = 100;
    const int count = width * height * layers * 4;

    std::ifstream ifs(std::string(PROJECT_ROOT) + "/resources/" + file, std::ifstream::in | std::ifstream::binary);
    std::vector<float> data(count);
    ifs.read((char*)data.data(), count * sizeof(float));

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 512, 512, 100, 0, GL_RGBA, GL_FLOAT, data.data());

    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}