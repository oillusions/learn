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

using namespace std;
namespace fs = filesystem;

fs::path modelPath = "resource/model/model.obj";

GLuint gui_fbo{};
GLuint gui_texture{};
GLuint gui_rbo{};
Camera gui_camera;

int fWidth, fHeight;


TestRenderCube::TestRenderCube(GLFWwindow* window):
    window(window),
    rootNode(Node<Transform>("root"))
{
    glfwSetWindowUserPointer(window, this);
    proj = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    data = stbi_load("resource/texture/texture.jpg", &width, &height, &nrChannels, 0);

    camera.init(window, ebus);

    fWidth = 800;
    fHeight = 600;

    camera._center_x = 800 / 2;
    camera._center_y = 600 / 2;
}

TestRenderCube::~TestRenderCube() {
    glDeleteTextures(1, &texture);
}


void TestRenderCube::init() {
    glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);


    glGenFramebuffers(1, &gui_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gui_fbo);

    glViewport(0, 0, fWidth, fHeight);

    glGenTextures(1, &gui_texture);
    glBindTexture(GL_TEXTURE_2D, gui_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fWidth, fHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gui_texture, 0);

    glGenRenderbuffers(1, &gui_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, gui_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fWidth, fHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gui_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cerr << "错误: 帧缓冲创建失败" << endl;
    }

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    map<string, VertexLayout<float>> modelVertices = ModelParser::ObjModelLoader(fileLoader(modelPath));

    for (auto& e : modelVertices) {
        models.emplace(piecewise_construct,
            forward_as_tuple(e.first),
            forward_as_tuple(e.first, rootNode.addChild(e.first), std::move(e.second), ebus)
        );
    }
    auto gui_face = VertexLayout<float>::builder()
        .appendElement("vertices", 3)
        .appendElement("texCoord", 2)
        .attachSource("vertices", {-1.0, 1.0, 0.0,  -1.0, -1.0, 0.0,  1.0, -1.0, 0.0,  1.0, 1.0, 0.0})
        .attachSource("texCoord", {0.0, 1.0,  0.0, 0.0,  1.0, 0.0,  1.0, 1.0})
        .attachIndices(vector<unsigned int>{0, 0, 1, 1, 2, 2,  2, 2, 3, 3, 0, 0})
        .build();
    models.emplace(piecewise_construct, forward_as_tuple("GUI"), forward_as_tuple("GUI", rootNode.addChild("GUI"), std::move(gui_face), ebus));
    for (auto& model : models) {
        model.second.init();
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data == nullptr) {
        cerr << "纹理加载失败" << endl;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    camera._position = {0.0f, -0.0f, 1.0f};

    stbi_image_free(data);
}

void TestRenderCube::render(double delta) {
    _delta = delta;
    camera._delta = delta;

    glm::mat4 gui_proj = glm::perspective(glm::radians(90.0f), static_cast<float>(fWidth) / static_cast<float>(fHeight), 0.001f, 100.0f);
    cout << "w:" << fWidth << ", h:" << fHeight << endl;

    const glm::mat4& cameraMatrix = camera.viewMatrix();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.78431372f, 0.78431372f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, gui_fbo);
    glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for (auto& model : models) {
        if (model.first != "GUI") {
            glBindFramebuffer(GL_FRAMEBUFFER, gui_fbo);
            glBindTexture(GL_TEXTURE_2D, texture);
            model.second.render(delta, gui_proj, cameraMatrix);
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, gui_texture);
    models.at("GUI").render(delta, proj, cameraMatrix);
    glDisable(GL_BLEND);
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
    fWidth = width;
    fHeight = height;
    glBindFramebuffer(GL_FRAMEBUFFER, gui_fbo);
    glViewport(0, 0, width, height);
    FrameSize_Event content{window, width, height};
    ebus.publish("frame-size-callback", content);

    proj = glm::perspective(glm::radians(90.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    render(_delta);
    glfwSwapBuffers(window);
}

void TestRenderCube::onScrollCallback(double x_offset, double y_offset) {
    MouseScroll_Event content{x_offset, y_offset};
    ebus.publish("mouse-scroll-callback", content);
}


void TestRenderCube::onKeyCallback(int key, int scancode, int action, int mods) {
    Keyboard_Event content{window, key, scancode, action, mods};
    ebus.publish("keyboard-callback", content);
    switch (key) {
        case GLFW_KEY_ESCAPE: {
            glfwSetWindowShouldClose(window, true);
            break;
        }
        case GLFW_KEY_R: {
            camera._perspective = {1.0f, 0.0f, 0.0f, 0.0f};
            camera._position = {0.0f, 0.25f, 1.0f};
            break;
        }
        case GLFW_KEY_SPACE: {
            break;
        }
        default: ;
    }
}

void TestRenderCube::onMouseMoveCallback(double x, double y) {
    MouseMove_Event content{window, x, y};
    ebus.publish("mouse-move-callback", content);
}

void TestRenderCube::onMouseButtonCallback(int button, int action, int mods) {
    MouseButton_Event content{button, action, mods};
    ebus.publish("mouse-button-callback", content);
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
        instance->onMouseMoveCallback(x, y);
    }
}

void TestRenderCube::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (auto* instance = static_cast<TestRenderCube *>(glfwGetWindowUserPointer(window))) {
        instance->onMouseButtonCallback(button, action, mods);
    }
}
