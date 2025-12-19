#include "TestRenderCode.h"

#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <ModelParser.h>
#include <glm/gtc/quaternion.hpp>


using namespace std;
namespace fs = filesystem;

fs::path modelPath = "resource/model/model.obj";

double center_x{0}, center_y{0};

TestRenderCube::TestRenderCube(GLFWwindow* window): window(window),
    vertexShader(Shader(Shader::Vertex, fileLoader(vertexPath))),
    fragmentShader(Shader(Shader::Fragment, fileLoader(fragmentPath))),
    bufferLayout(VertexLayout<float>::builder()
        .appendElement("vertices", 3)
        .appendElement("texCoord", 2)
        .build())
{
    proj = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    data = stbi_load("resource/texture/texture.jpg", &width, &height, &nrChannels, 0);
    glfwSetWindowUserPointer(window, this);
    bufferLayout = ModelParser::ObjModelLoader(fileLoader(modelPath));
    center_x = 800 / 2;
    center_y = 600 / 2;
}

TestRenderCube::~TestRenderCube() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture);
}


void TestRenderCube::init() {
    glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);


    vv = bufferLayout.ExpandIndices();
    vi = bufferLayout.bufferOfIndices();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenTextures(1, &texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,   vv.size() * sizeof(float), vv.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vi.size() * sizeof(unsigned int), vi.data(), GL_STATIC_DRAW);

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

    bufferLayout.bufferLayoutDeclaration();

    program.attach(vertexShader);
    program.attach(fragmentShader);
    (void) program.link();

    auto& modelTran = testModelNode.get();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    modelTran.rotate({glm::radians(-90.0f), glm::radians(122.5f), 0.0f});
    modelTran.translate({0.0f, 0.0f, -10.0f});
    modelTran.scale({0.5f, 0.5f, 0.5f});

    glEnable(GL_DEPTH_TEST);
    camera.translate({0.0f, 0.0f, -15.0f});

    stbi_image_free(data);
}

void TestRenderCube::render(double delta) {
    _delta = delta;
    static auto dataset = testModelNode.getTraceToRoot(true);
    glClearColor(0.78431372f, 0.78431372f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    if (isRotate) {
        testNode.get().rotate({0.0f, glm::radians(90.0f * _delta), 0.0f});
    }
    program["Transform"].setMat4(proj * camera.getMatrix() * Transform::worldMatrix(dataset));
    glDrawElements(GL_TRIANGLES, vi.size(), GL_UNSIGNED_INT, nullptr);
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

    return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

void TestRenderCube::onFrameBufferSizeCallback(int width, int height) {
    glViewport(0, 0, width, height);
    proj = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);
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
            camera.translate(glm::vec4({0.0, 0.0, 45.0  * _delta, 1.0}) * glm::mat4_cast(glm::quat(camera.getRotation())));
            break;
        }
        case GLFW_KEY_S: {
            camera.translate(glm::vec4({0.0, 0.0, -45.0  * _delta, 1.0}) * glm::mat4_cast(glm::quat(camera.getRotation())));
            break;
        }
        case GLFW_KEY_A: {
            camera.translate(glm::vec4({45.0  * _delta, 0.0, 0.0, 1.0}) * glm::mat4_cast(glm::quat(camera.getRotation())));
            break;
        }
        case GLFW_KEY_D: {
            camera.translate(glm::vec4({-45.0  * _delta, 0.0, 0.0, 1.0}) * glm::mat4_cast(glm::quat(camera.getRotation())));
            break;
        }
        case GLFW_KEY_R: {
            testNode.get().setRotate({0, 0, 0});
            testNode.get().setTranslate({0, 0, 0});
            camera.setRotate({0, 0, 0});
            camera.setTranslate({0, 0, -15.0f});
            break;
        }
        case GLFW_KEY_SPACE: {}
            if (action == GLFW_RELEASE) isRotate = !isRotate;
            break;
        default: ;
    }
}

void TestRenderCube::onMouseCallback(double x, double y) {
    double offset_x{}, offset_y{};
    offset_x = x - center_x;
    offset_y = y - center_y;
    glfwSetCursorPos(window, center_x, center_y);
    camera.rotate({glm::radians( 5.0 *offset_y * _delta), glm::radians(5.0 * offset_x * _delta), 0.0f});
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
