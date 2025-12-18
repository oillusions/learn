#pragma once
#include "glad/glad.h"
#include <string>

class Shader {
    public:
        enum ShaderType {
            Vertex = GL_VERTEX_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            Fragment = GL_FRAGMENT_SHADER,
        };

        explicit Shader(ShaderType shaderType, const std::string& shaderSource);
        ~Shader();

        operator unsigned int() const {
            return _location;
        }
    private:
        unsigned int _location;
};
