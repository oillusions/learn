#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <TestRenderCode.h>
#include <GlobalLogger.hpp>

using namespace std;
namespace fs = filesystem;

double deltaTime{};
double lastTime{};

GLFWwindow* window{nullptr};

void render() {
    glfwMakeContextCurrent(window);
    EventBus ebus{};

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glog.log(DefaultLevel::Error, "glad初始化失败");
        return;
    }

    TestRenderCode test(window, ebus);
    test.init();
    glfwSwapInterval(1);

    glViewport(0, 0, 800, 600);

    double currentTime{};
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        test.render(deltaTime);
        glfwSwapBuffers(window);
    }
}

int main() {

    glog.log(DefaultLevel::Info, "测试");

    stbi_set_flip_vertically_on_load(true);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Learn", nullptr, nullptr);
    if (window == nullptr) {
        glog.log(DefaultLevel::Error, "窗口创建失败");
        glfwTerminate();
        return -1;
    }

    thread openglThread(render);

    openglThread.detach();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}