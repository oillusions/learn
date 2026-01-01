#pragma once
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <Transform.h>
#include <Node.hpp>
#include <VertexLayout.hpp>
#include <Model.h>
#include <EventBus.hpp>

#include "Camera.h"

class TestRenderCode {
    public:
        explicit TestRenderCode(GLFWwindow* window, EventBus& ebus);
        ~TestRenderCode();

        void init();
        void render(double delta);

        void static frameBuffer_size_callback(GLFWwindow *window, int width, int height);
        void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void static scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
        void static mouse_callback(GLFWwindow* window, double x, double y);
        void static mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        void onFrameBufferSizeCallback(int width, int height);
        void onKeyCallback(int key, int scancode, int action, int mods);
        void onScrollCallback(double x_offset, double y_offset);
        void onMouseMoveCallback(double x, double y);
        void onMouseButtonCallback(int button, int action, int mods);


        static std::string fileLoader(const std::filesystem::path& path);

    private:
        GLFWwindow* window;
        EventBus& ebus;
        unsigned int texture{};
        int texWidth{}, texHeight{}, texNrChannels{};
        int frameWidth{}, frameHeight{};
        unsigned char* data{};
        std::map<std::string, Model> models;
        double _delta{};
        Node<Transform> rootNode;
        glm::mat4 proj{1.0f};
        Camera camera{};
};
