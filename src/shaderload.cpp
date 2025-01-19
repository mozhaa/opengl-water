#include "shaderload.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

static void make_lower(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
}

static GLenum get_type(std::filesystem::path source_file) {
    std::string ext = source_file.extension();
    make_lower(ext);
    if (ext == ".vert")
        return GL_VERTEX_SHADER;
    if (ext == ".frag")
        return GL_FRAGMENT_SHADER;
    if (ext == ".geom")
        return GL_GEOMETRY_SHADER;
    if (ext == ".comp")
        return GL_COMPUTE_SHADER;
    throw std::runtime_error("Unknown shader type: \"" + ext + "\"");
};

std::string get_shader_source(std::filesystem::path source_file) {
    std::ifstream fin(source_file);
    if (!fin)
        throw std::runtime_error("Can't open file: " + source_file.string());
    std::stringstream result;
    result << fin.rdbuf();
    return result.str();
}

GLuint load_shader(std::filesystem::path source_file) {
    GLuint result = glCreateShader(get_type(source_file));
    std::string source = get_shader_source(source_file);
    const char* source_c = source.c_str();
    glShaderSource(result, 1, &source_c, nullptr);
    glCompileShader(result);
    GLint status;
    glGetShaderiv(result, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint info_log_length;
        glGetShaderiv(result, GL_INFO_LOG_LENGTH, &info_log_length);
        std::string info_log(info_log_length, '\0');
        glGetShaderInfoLog(result, info_log.size(), nullptr, info_log.data());
        throw std::runtime_error(source_file.string() + ": Shader compilation failed: " + info_log);
    }
    return result;
}

std::vector<GLuint> load_shaders(std::vector<std::filesystem::path> source_files) {
    std::vector<GLuint> result(source_files.size());
    for (int i = 0; i < source_files.size(); ++i)
        result[i] = load_shader(source_files[i]);
    return result;
}

GLuint create_program(std::vector<std::filesystem::path> source_files) {
    GLuint result = glCreateProgram();
    for (auto shader: load_shaders(source_files))
        glAttachShader(result, shader);
    glLinkProgram(result);

    GLint status;
    glGetProgramiv(result, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint info_log_length;
        glGetProgramiv(result, GL_INFO_LOG_LENGTH, &info_log_length);
        std::string info_log(info_log_length, '\0');
        glGetProgramInfoLog(result, info_log.size(), nullptr, info_log.data());
        throw std::runtime_error("Program linkage failed: " + info_log);
    }

    return result;
}
