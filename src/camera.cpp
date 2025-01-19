#include "camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

camera_settings::camera_settings(int width, int height)
    : width(width),
      height(height),
      view_angle(0.65f),
      camera_distance(1.6f),
      camera_height(0.0f),
      camera_rotation(1.6f),
      horizontal_rotation_speed(0.8f),
      vertical_rotation_speed(0.3f),
      scale_speed(2.f),
      near(0.01f),
      far(100.f) {
    update_matrices();
}

inline glm::vec3 dir_to_vec(glm::vec2 dir) {
    return {
        cos(dir.y) * sin(dir.x),
        sin(dir.y),
        cos(dir.y) * cos(dir.x),
    };
}

void camera_settings::update(std::map<SDL_Keycode, bool> &button_down, float dt) {
    if (button_down[SDLK_UP])
        camera_distance -= scale_speed * dt;
    if (button_down[SDLK_DOWN])
        camera_distance += scale_speed * dt;

    if (button_down[SDLK_LEFT])
        camera_rotation -= horizontal_rotation_speed * dt;
    if (button_down[SDLK_RIGHT])
        camera_rotation += horizontal_rotation_speed * dt;

    if (button_down[SDLK_w])
        view_angle += vertical_rotation_speed * dt;
    if (button_down[SDLK_s])
        view_angle -= vertical_rotation_speed * dt;

    update_matrices();
}

void camera_settings::update_matrices() {
    view = glm::translate(glm::mat4(1.f), {0.f, -camera_height, -camera_distance});
    view = glm::rotate(view, view_angle, {1.f, 0.f, 0.f});
    view = glm::rotate(view, camera_rotation, {0.f, 1.f, 0.f});

    projection = glm::perspective(glm::pi<float>() / 2.f, (1.f * width) / height, near, far);

    camera_position = (glm::inverse(view) * glm::vec4(0.f, 0.f, 0.f, 1.f)).xyz();

    view_projection_inverse = glm::inverse(projection * view);
}
