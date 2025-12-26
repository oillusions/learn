#pragma once
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "EventBus.hpp"
#include "EventTypes.hpp"

class Camera {
    public:
        double _delta{};
        glm::vec3 _position{0.0, 0.0, 0.0};
        glm::quat _perspective{1.0, 0.0, 0.0, 0.0};
        double _center_x{}, _center_y{};
        bool _bindCursor{true};

        Camera() = default;
        ~Camera() = default;

        glm::mat4 viewMatrix() const;

        void init(GLFWwindow* window, const EventBus& ebus);

        void reset();

        void handleCameraMove(const Keyboard_Event& content);
        void handleFrameCenterUpdate(const FrameSize_Event& content);
        void handleCameraPerspective(const MouseMove_Event& content);
};
