#pragma once
#include <string>
#include <glm/glm.hpp>

class ShaderUniform {
    public:
        ShaderUniform(): _location(-1), _type(0) {};
        ShaderUniform(int id, unsigned int type);

        ~ShaderUniform() = default;

        operator int() const {
            return _location;
        }

        void setFloat(float value);
        void setInt(int value);
        void setBool(bool value);
        void setF4(const glm::vec4& value);
        void setMat4(const glm::mat4& value) const;
    private:
        const int _location;
        const unsigned int _type;

};
