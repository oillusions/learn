#pragma once
#include <glm/glm.hpp>

class Transform {
    public:
        Transform() = default;
        ~Transform() = default;

        Transform& origin(glm::vec3 vec);
        Transform& translate(glm::vec3 vec);
        Transform& scale(glm::vec3 vec);
        Transform& rotate(glm::vec3 vec);

        Transform& setTranslate(const glm::vec3& vec);
        Transform& setScale(const glm::vec3& vec);
        Transform& setRotate(const glm::vec3& vec);

        const glm::vec3& getPosition();
        const glm::vec3& getRotation();
        const glm::vec3& getScale();
        const glm::vec3& getOrigin() const;

        bool isDirty() const;

        [[nodiscard]] glm::mat4 getMatrix() const;

        operator glm::mat4(){
            return getMatrix();
        }
    private:
        glm::vec3 _position{0.0};
        glm::vec3 _rotation{0.0};
        glm::vec3 _scale{1.0, 1.0, 1.0};
        glm::vec3 _origin{0.0};
        mutable glm::mat4 _cacheMatrix{1.0f};
        mutable bool _isDirty{true};
};
