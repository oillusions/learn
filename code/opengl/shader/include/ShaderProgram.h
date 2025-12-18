#pragma once
#include <map>

#include "Shader.h"
#include "ShaderUniform.h"

class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void attach(const Shader& shader) const;
        [[nodiscard]] bool link();
        void use() const;



        operator unsigned int() const {
            return _location;
        }

        ShaderUniform& operator[] (const std::string& name) {
            return uniformMap.at(name);
        }

        ShaderUniform& operator[] (const char* name) {
            return uniformMap.at(std::string(name));
        }

    private:
        unsigned int _location;
        bool isLink {};
        std::map<std::string, ShaderUniform> uniformMap;

        void initUniformMap();
        void deInitUniformMap();

};
