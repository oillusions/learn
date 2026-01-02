#include "TestRenderCode.h"

#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/gtc/quaternion.hpp>

#include <EventTypes.hpp>
#include <ModelParser.h>
#include <Bezier.h>
#include <GlobalLogger.hpp>

using namespace std;
namespace fs = filesystem;

fs::path modelPath = "resource/model/model.obj";


TestRenderCode::TestRenderCode(GLFWwindow* window, EventBus& ebus):
    window(window),
    ebus(ebus),
    rootNode(Node<Transform>("root")),
    texture(arm.find<Texture>("texture.default"))
{
    glfwSetWindowUserPointer(window, this);

    camera.init(window, ebus);

    frameWidth = 800;
    frameHeight = 600;

    camera._center_x = 800 / 2;
    camera._center_y = 600 / 2;
}


void TestRenderCode::init() {
    glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    map<string, VertexLayout<float>> modelVertices = ModelParser::ObjModelLoader(resource::utils::readFileToStr(modelPath));

    for (auto& e : modelVertices) {
        models.emplace(piecewise_construct,
            forward_as_tuple(e.first),
            forward_as_tuple(e.first, rootNode.addChild(e.first), std::move(e.second), ebus)
        );
    }
    for (auto& model : models) {
        model.second.init();
    }

    glEnable(GL_DEPTH_TEST);
    camera._position = {0.0f, -0.0f, 1.0f};
    proj = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    stbi_image_free(data);
}

void TestRenderCode::render(double delta) {
    _delta = delta;
    camera._delta = delta;

    glViewport(0, 0, frameWidth, frameHeight);
    proj = glm::perspective(glm::radians(90.0f), static_cast<float>(frameWidth) / static_cast<float>(frameHeight), 0.1f, 100.0f);

    const glm::mat4& cameraMatrix = camera.viewMatrix();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.78431372f, 0.78431372f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    for (auto& model : models) {
        glBindTexture(GL_TEXTURE_2D, texture);
        model.second.render(delta, proj, cameraMatrix);
    }
}

void TestRenderCode::onFrameBufferSizeCallback(int width, int height) {
    FrameSize_Event content{window, width, height};
    ebus.publish("frame-size-callback", content);
    this->frameWidth = width == 0 ? 1 : width;
    this->frameHeight = height == 0 ? 1 : height;
}

void TestRenderCode::onScrollCallback(double x_offset, double y_offset) {
    MouseScroll_Event content{x_offset, y_offset};
    ebus.publish("mouse-scroll-callback", content);
}


void TestRenderCode::onKeyCallback(int key, int scancode, int action, int mods) {
    Keyboard_Event content{window, key, scancode, action, mods};
    ebus.publish("keyboard-callback", content);
    switch (key) {
        case GLFW_KEY_ESCAPE: {
            glfwSetWindowShouldClose(window, true);
            break;
        }
        case GLFW_KEY_R: {
            camera._perspective = {1.0f, 0.0f, 0.0f, 0.0f};
            camera._position = {0.0f, 0.0f, 1.0f};
            break;
        }
        case GLFW_KEY_SPACE: {
            break;
        }
        default: ;
    }
}

void TestRenderCode::onMouseMoveCallback(double x, double y) {
    MouseMove_Event content{window, x, y};
    ebus.publish("mouse-move-callback", content);
}

void TestRenderCode::onMouseButtonCallback(int button, int action, int mods) {
    MouseButton_Event content{button, action, mods};
    ebus.publish("mouse-button-callback", content);
}


void TestRenderCode::frameBuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (auto* instance = static_cast<TestRenderCode *>(glfwGetWindowUserPointer(window))) {
        instance->onFrameBufferSizeCallback(width, height);
    }
}

void TestRenderCode::scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    if (auto* instance = static_cast<TestRenderCode *>(glfwGetWindowUserPointer(window))) {
        instance->onScrollCallback(x_offset, y_offset);
    }
}

void TestRenderCode::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (auto* instance = static_cast<TestRenderCode *>(glfwGetWindowUserPointer(window))) {
        instance->onKeyCallback(key, scancode, action, mods);
    }
}

void TestRenderCode::mouse_callback(GLFWwindow *window, double x, double y) {
    if (auto* instance = static_cast<TestRenderCode *>(glfwGetWindowUserPointer(window))) {
        instance->onMouseMoveCallback(x, y);
    }
}

void TestRenderCode::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (auto* instance = static_cast<TestRenderCode *>(glfwGetWindowUserPointer(window))) {
        instance->onMouseButtonCallback(button, action, mods);
    }
}
