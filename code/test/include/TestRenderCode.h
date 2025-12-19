#pragma once
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <ShaderProgram.h>
#include <Transform.h>
#include <Node.hpp>
#include <VertexLayout.hpp>

class TestRenderCube {
    public:
        explicit TestRenderCube(GLFWwindow* window);
        ~TestRenderCube();

        void init();
        void render(double delta);

        void static frameBuffer_size_callback(GLFWwindow *window, int width, int height);
        void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void static scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
        void static mouse_callback(GLFWwindow* window, double x, double y);

        void onFrameBufferSizeCallback(int width, int height);
        void onKeyCallback(int key, int scancode, int action, int mods);
        void onScrollCallback(double x_offset, double y_offset);
        void onMouseCallback(double x, double y);


        static std::string fileLoader(const std::filesystem::path& path);

    private:
        GLFWwindow* window;

        const std::filesystem::path vertexPath = "resource/shader/vertex.glsl";
        const std::filesystem::path fragmentPath = "resource/shader/fragment.glsl";
        unsigned int vao{};
        unsigned int vbo{};
        unsigned int ebo{};
        Shader vertexShader;
        Shader fragmentShader;
        unsigned int texture{};
        int width{}, height{}, nrChannels{};
        unsigned char* data{};
        ShaderProgram program;
        VertexLayout<float> bufferLayout;
        double _delta{};

        std::vector<float> vv;
        std::vector<unsigned int> vi;

        Node<Transform> rootNode = Node<Transform>("root");
        Node<Transform>& testNode = rootNode.addChild("test");
        Node<Transform>& testModelNode = testNode.addChild("model");
        glm::mat4 proj{1.0f};
        Transform camera;
        bool isRotate = false;
};
