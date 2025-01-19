#pragma once

#ifdef WIN32
#include <SDL.h>
#undef main
#else
#include <SDL2/SDL.h>
#endif

#include <vector>
#include <string>
#include <map>

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <GL/glew.h>

struct caustic_drawer {
    GLuint program, FBO, caustic_texture, pool_texture, copy_program, VAO;
    int width, height;
    float factor, power;

    caustic_drawer(std::string texture_name);
    void update(GLuint water_VAO, std::vector<uint32_t>& water_indices, GLuint heights_texture, glm::vec3 sun_direction, int frame_idx);
    void set_parameters(std::map<SDL_Keycode, bool>& button_down, float dt);
};