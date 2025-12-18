#include "TestRenderCode.h"

#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <ModelParser.h>

using namespace std;
namespace fs = filesystem;

fs::path modelPath = "resource/model/model.obj";

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

    testModelNode.getTransform().rotate({glm::radians(-90.0f), glm::radians(122.5f), 0.0f});
    testModelNode.getTransform().translate({0.0f, 0.0f, -10.0f});
    testModelNode.getTransform().scale({0.5f, 0.5f, 0.5f});

    glEnable(GL_DEPTH_TEST);
    camera.origin({0.0f, 0.0f, -2.0f});
    camera.translate({0.0f, 0.0f, -15.0f});

    stbi_image_free(data);
}

void TestRenderCube::render() {
    glClearColor(0.78431372f, 0.78431372f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    if (isRotate) {
        testNode.getTransform().rotate({0.0f, glm::radians(0.5f), 0.0f});
    }
    program["Transform"].setMat4(proj * camera.getMatrix() * testModelNode.getWorldMatrix());
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
    render();
    glfwSwapBuffers(window);
}

void TestRenderCube::onScrollCallback(double x_offset, double y_offset) {
    camera.translate({0.0f, 0.0f, y_offset});
}


void TestRenderCube::onKeyCallback(int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE: {
            glfwSetWindowShouldClose(window, true);
            break;
        }
        case GLFW_KEY_W: {
            rootNode.getTransform().rotate({glm::radians(-1.0f), 0.0f, 0.0f});
            break;
        }
        case GLFW_KEY_S: {
            rootNode.getTransform().rotate({glm::radians(1.0f), 0.0f, 0.0f});
            break;
        }
        case GLFW_KEY_A: {
            rootNode.getTransform().rotate({0.0f, glm::radians(-1.0f), 0.0f});
            break;
        }
        case GLFW_KEY_D: {
            rootNode.getTransform().rotate({0.0f, glm::radians(1.0f), 0.0f});
            break;
        }
        case GLFW_KEY_Q: {
            rootNode.getTransform().rotate({0.0f, 0.0f, glm::radians(-1.0f)});
            break;
        }
        case GLFW_KEY_E: {
            rootNode.getTransform().rotate({0.0f, 0.0f, glm::radians(1.0f)});
            break;
        }
        case GLFW_KEY_R: {
            rootNode.getTransform().setRotate({0, 0, 0});
            rootNode.getTransform().setTranslate({0, 0, 0});
            break;
        }
        case GLFW_KEY_SPACE: {}
            if (action == GLFW_RELEASE) isRotate = !isRotate;
            break;
        default: ;
    }
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
