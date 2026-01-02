#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <TestRenderCode.h>
#include <GlobalLogger.hpp>
#include <Resource.hpp>
#include <ResourceTypes.hpp>

using namespace std;
namespace fs = filesystem;

double deltaTime{};
double lastTime{};

GLFWwindow* window{nullptr};

void render() {
    glog.log<DefaultLevel::Info>("渲染线程已启动");
    glfwMakeContextCurrent(window);
    EventBus ebus{};

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glog.log(DefaultLevel::Error, "glad初始化失败");
        return;
    }

    arm.load<Texture>("texture.default", "resource/texture/texture.jpg");

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
    glog.log<DefaultLevel::Info>("渲染线程已结束");
}

int main() {
    stbi_set_flip_vertically_on_load(true);

    globalLogger::_minLevel = DefaultLevel::Debug;
    glog.log(DefaultLevel::Info, "程序已启动");


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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    openglThread.join();
    glfwTerminate();

    glog.log<DefaultLevel::Info>("程序已结束");
    return 0;
}