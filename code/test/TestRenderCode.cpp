#include "TestRenderCode.h"

#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/gtc/quaternion.hpp>

#include <ModelParser.h>
#include <Bezier.h>


using namespace std;
namespace fs = filesystem;

fs::path modelPath = "resource/model/model.obj";

double center_x{0}, center_y{0};

TestRenderCube::TestRenderCube(GLFWwindow* window):
    window(window),
    rootNode(Node<Transform>("root"))
{
    proj = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    data = stbi_load("resource/texture/texture.jpg", &width, &height, &nrChannels, 0);
    glfwSetWindowUserPointer(window, this);
    center_x = 800 / 2;
    center_y = 600 / 2;
}

TestRenderCube::~TestRenderCube() {
    glDeleteTextures(1, &texture);
}


void TestRenderCube::init() {
    glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    map<string, VertexLayout<float>> modelVertices = ModelParser::ObjModelLoader(fileLoader(modelPath));

    for (auto& e : modelVertices) {
        cout << "modelName: " << e.first << endl;
        e.second.test();
        models.emplace(piecewise_construct,
            forward_as_tuple(e.first),
            forward_as_tuple(e.first, rootNode.addChild(e.first), std::move(e.second))
        );
    }
    for (auto& model : models) {
        model.second.init();
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data == nullptr) {
        cerr << "纹理加载失败" << endl;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


    glEnable(GL_DEPTH_TEST);
    camera.translate({0.0f, 0.25f, 1.0f});
    camera.configInverse();

    stbi_image_free(data);
}

void TestRenderCube::render(double delta) {
    _delta = delta;

    // glClearColor(0.78431372f, 0.78431372f, 1.0f, 1.00f);
    glClearColor(0.0f, 0.0f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static Bezier test({0.25 ,0.1}, {0.25, 1.0});
    const glm::mat4& cameraMatrix = camera.getMatrix();

    for (auto& model : models) {
        glBindTexture(GL_TEXTURE_2D, texture);
        model.second.render(delta, proj, cameraMatrix);
    }
}

string TestRenderCube::fileLoader(const fs::path& path) {
    if (!fs::exists(path)) {
        cerr << "文件不存在: " << path << endl;
        return {};
    }

    ifstream file(path);
    if (!file.is_open()) {
        cerr << "文件无法打开: " << path << endl;
        return {};
    }
    string out{istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
    file.close();
    return out;
}

void TestRenderCube::onFrameBufferSizeCallback(int width, int height) {
    glViewport(0, 0, width, height);
    proj = glm::perspective(glm::radians(90.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    center_x = width / 2;
    center_y = height / 2;
    render(_delta);
    glfwSwapBuffers(window);
}

void TestRenderCube::onScrollCallback(double x_offset, double y_offset) {
}


void TestRenderCube::onKeyCallback(int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE: {
            glfwSetWindowShouldClose(window, true);
            break;
        }
        case GLFW_KEY_W: {
            camera.translate(glm::vec4({0.0, 0.0, -45.0  * _delta, 1.0}) * glm::mat4_cast(glm::inverse(camera.getRotation())));
            break;
        }
        case GLFW_KEY_S: {
            camera.translate(glm::vec4({0.0, 0.0, 45.0  * _delta, 1.0}) * glm::mat4_cast(glm::inverse(camera.getRotation())));
            break;
        }
        case GLFW_KEY_A: {
            camera.translate(glm::vec4({-45.0  * _delta, 0.0, 0.0, 1.0}) * glm::mat4_cast(glm::inverse(camera.getRotation())));
            break;
        }
        case GLFW_KEY_D: {
            camera.translate(glm::vec4({45.0  * _delta, 0.0, 0.0, 1.0}) * glm::mat4_cast(glm::inverse(camera.getRotation())));
            break;
        }
        case GLFW_KEY_R: {

            camera.setRotate({0, 0, 0})
                .setTranslate({0.0f, 0.25f, 1.0f});
            break;
        }
        case GLFW_KEY_SPACE: {
            break;
        }
        default: ;
    }
}

void TestRenderCube::onMouseCallback(double x, double y) {
    double offset_x{}, offset_y{};

    offset_x = center_x - x;
    offset_y = center_y - y;
    glfwSetCursorPos(window, center_x, center_y);
    camera.rotate({glm::radians( 5.0 * offset_y * _delta), 0.0f, 0.0f});
    camera.rotate({0.0f, glm::radians(5.0 * offset_x * _delta), 0.0f});
}


void TestRenderCube::frameBuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (auto* instance = static_cast<TestRenderCube *>(glfwGetWindowUserPointer(window))) {
        instance->onFrameBufferSizeCallback(width, height);
    }
}

void TestRenderCube::scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    if (auto* instance = static_cast<TestRenderCube *>(glfwGetWindowUserPointer(window))) {
        instance->onScrollCallback(x_offset, y_offset);
    }
}

void TestRenderCube::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (auto* instance = static_cast<TestRenderCube *>(glfwGetWindowUserPointer(window))) {
        instance->onKeyCallback(key, scancode, action, mods);
    }
}

void TestRenderCube::mouse_callback(GLFWwindow *window, double x, double y) {
    if (auto* instance = static_cast<TestRenderCube *>(glfwGetWindowUserPointer(window))) {
        instance->onMouseCallback(x, y);
    }
}
