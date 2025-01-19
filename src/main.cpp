#ifdef WIN32
#include <SDL.h>
#undef main
#else
#include <SDL2/SDL.h>
#endif

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include <GL/glew.h>

#include <stdexcept>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

#include "pool.hpp"
#include "camera.hpp"
#include "lighting_settings.hpp"
#include "envmap.hpp"
#include "water.hpp"


std::string to_string(std::string_view str) {
    return std::string(str.begin(), str.end());
}

void sdl2_fail(std::string_view message) {
    throw std::runtime_error(to_string(message) + SDL_GetError());
}

void glew_fail(std::string_view message, GLenum error) {
    throw std::runtime_error(to_string(message) + reinterpret_cast<const char *>(glewGetErrorString(error)));
}

void sdl_init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        sdl2_fail("SDL_Init: ");
}

void set_gl_attributes() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

SDL_Window* create_window() {
    SDL_Window * window = SDL_CreateWindow("Graphics course practice 5",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    if (!window)
        sdl2_fail("SDL_CreateWindow: ");

    return window;
}

SDL_GLContext create_context(SDL_Window* window) {
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
        sdl2_fail("SDL_GL_CreateContext: ");
    return gl_context;
}

void glew_init() {
    if (auto result = glewInit(); result != GLEW_NO_ERROR)
        glew_fail("glewInit: ", result);

    if (!GLEW_VERSION_3_3)
        throw std::runtime_error("OpenGL 3.3 is not supported");
}

int main(int argc, char* argv[]) try {
    START_EASYLOGGINGPP(argc, argv);
    sdl_init();
    set_gl_attributes();
    auto window = create_window();
    auto context = create_context(window);
    glew_init();

    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    auto last_frame_start = std::chrono::high_resolution_clock::now();
    float time = 0.f;
    std::map<SDL_Keycode, bool> button_down;

    pool P("pool.jpg");
    water W(128, 128);
    camera_settings camera(width, height);
    environment_map envmap("forest.jpg");
    lighting_settings pool_lighting = {
        glm::vec3(0.1),
        glm::vec3(0.1),
        glm::vec3(0.6),
        glm::vec3(1.0, 1.0, 0.0),
        glm::vec3(0.8, 1.0, 0.9),
        32.0,
    };
    lighting_settings water_lighting = {
        glm::vec3(0.4),
        glm::vec3(0.1),
        glm::vec3(1.3),
        glm::vec3(1.0, 1.0, 0.0),
        glm::vec3(0.8, 1.0, 0.9),
        32.0,
    };

    glClearColor(0.8f, 0.8f, 1.f, 0.f);

    bool paused = false;
    bool running = true;
    while (running) {
        for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_WINDOWEVENT: switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                width = event.window.data1;
                height = event.window.data2;
                camera.width = width;
                camera.height = height;
                glViewport(0, 0, width, height);
                break;
            }
            break;
        case SDL_KEYDOWN:
            button_down[event.key.keysym.sym] = true;

            if (event.key.keysym.sym == SDLK_SPACE)
                paused = !paused;

            break;
        case SDL_KEYUP:
            button_down[event.key.keysym.sym] = false;
            break;
        }

        if (!running)
            break;
        
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_frame_start).count();
        last_frame_start = now;
        if (!paused) {
            time += dt;

            W.update_heights(time);
        }

        camera.update(button_down, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        envmap.draw(camera);
        P.draw(camera, pool_lighting);
        W.draw(camera, water_lighting, envmap, P);

        SDL_GL_SwapWindow(window);
    }

} catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
