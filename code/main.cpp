#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <filesystem>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "TestRenderCode.h"

using namespace std;
namespace fs = filesystem;

double deltaTime{};
double lastTime{};

int main() {
    stbi_set_flip_vertically_on_load(true);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Learn", nullptr, nullptr);
    if (window == nullptr) {
        cout << "窗口创建失败" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        cout << "glad初始化失败" << endl;
        return -1;
    }
    TestRenderCube test(window);
    test.init();
    glfwSwapInterval(1);

    glViewport(0, 0, 800, 600);

    double currentTime{};
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        test.render(deltaTime);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}