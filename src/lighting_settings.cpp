#include "lighting_settings.hpp"

void lighting_settings::set_uniforms(GLuint program) {
    glUniform3fv(glGetUniformLocation(program, "ambient_strength"), 1, reinterpret_cast<float *>(&ambient_strength));
    glUniform3fv(glGetUniformLocation(program, "diffuse_strength"), 1, reinterpret_cast<float *>(&diffuse_strength));
    glUniform3fv(glGetUniformLocation(program, "specular_strength"), 1, reinterpret_cast<float *>(&specular_strength));
    glUniform3fv(glGetUniformLocation(program, "sun_direction"), 1, reinterpret_cast<float *>(&sun_direction));
    glUniform3fv(glGetUniformLocation(program, "sun_color"), 1, reinterpret_cast<float *>(&sun_color));
    glUniform1f(glGetUniformLocation(program, "power"), power);
}