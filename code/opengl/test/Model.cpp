#include "Model.h"

#include <TestRenderCode.h>

#include "Bezier.h"
#include "EventTypes.hpp"

using namespace std;
namespace fs = filesystem;

const fs::path vertexPath = "resource/shader/vertex.glsl";
const fs::path fragmentPath = "resource/shader/fragment.glsl";

Model::Model(const std::string &name, Node<Transform>::IndexWrapper modelTransformNode , VertexLayout<float> modelVertices, const EventBus& ebus):
    _name(name),
    vertexShader(Shader(Shader::Vertex, TestRenderCube::fileLoader(vertexPath))),
    fragmentShader(Shader(Shader::Fragment, TestRenderCube::fileLoader(fragmentPath))),
    _modelVertices(std::move(modelVertices)),
    _modelRootNode(modelTransformNode),
    _modelInitTransform(_modelRootNode.addChild("ModelInitTransform")),
    _ebus(ebus) {
    // if (_name == "GUI") {
    //     glDeleteShader(fragmentShader);
    //     fragmentShader = Shader(Shader::Fragment, TestRenderCube::fileLoader("resource/shader/gui_fragment.glsl"));
    // }

}

Model::~Model() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Model::init() {
    vv = _modelVertices.ExpandIndices();
    vi = _modelVertices.bufferOfIndices();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,   vv.size() * sizeof(float), vv.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vi.size() * sizeof(unsigned int), vi.data(), GL_STATIC_DRAW);

    _modelVertices.bufferLayoutDeclaration();

    transformInit();

    _ebus.subscribe<Keyboard_Event>("keyboard-callback", [this](const Keyboard_Event& content) {
        if (content.key == GLFW_KEY_SPACE && content.action == GLFW_RELEASE) {
            this->k = true;
        }
    });

    // if (_name == "GUI") {
    //     _ebus.subscribe<FrameSize_Event>("frame-size-callback", [this](const FrameSize_Event& content) {
    //         _modelRootNode.get().setScale({, 1.0f});
    //     });
    // }

    program.attach(vertexShader);
    program.attach(fragmentShader);
    (void) program.link();
}

void Model::transformInit() {
    Transform& initTransform = _modelInitTransform.get();
    Transform& modelTransform = _modelRootNode.get();

    if (_name == "嘴") {
        initTransform.setTranslate({0.0f, 0.264522f, 0.303543f});
        modelTransform.origin({0.0f, 0.0f, 0.026443f});
        modelTransform.resetScale();
    } else if (_name == "眼白-左") {
        initTransform.setTranslate({-0.04f, 0.33398f, 0.255f});
    } else if (_name == "眼瞳-左") {
        initTransform.setTranslate({-0.04f, 0.33398f, 0.29f});
        modelTransform.origin({0.0f, 0.0f, 0.035f});
        modelTransform.rotate({glm::radians(20.0f), glm::radians(-15.0f), 0.0f});
    } else if (_name == "眉毛-左") {
        initTransform.setTranslate({-0.045f, 0.385f, 0.19f});
    } else if (_name == "翅膀-左") {
        initTransform.setTranslate({-0.521066f, 0.106066f, 0.127f});
        modelTransform.origin({-0.136066f, 0.0f, 0.0f});
        modelTransform.rotate({0.0f, 0.0f, glm::radians(60.0f)});
    } else if (_name == "眼白-右") {
        initTransform.setTranslate({0.04f, 0.33398f, 0.255f});
    } else if (_name == "眼瞳-右") {
        initTransform.setTranslate({0.04f, 0.33398f, 0.29f});
        modelTransform.origin({0.0f, 0.0f, 0.035f});
        modelTransform.rotate({glm::radians(-20.0f), glm::radians(15.0f), 0.0f});
    } else if (_name == "眉毛-右") {
        initTransform.setTranslate({0.045f, 0.385f, 0.19f});
    } else if (_name == "翅膀-右") {
        initTransform.setTranslate({0.521066f, 0.106066f, 0.127f});
        modelTransform.origin({0.136066f, 0.0f, 0.0f});
        modelTransform.rotate({0.0f, 0.0f, glm::radians(-60.0f)});
    } else if (_name == "GUI") {
        // initTransform.setTranslate({0.0f, 0.0f, 2.0f});
    }
}

void Model::render(double delta, const glm::mat4 &projection, const glm::mat4 &camera) {
    Bezier curve({0.0, 1.0}, {1.0, 0.0});

    if (_transformChain.empty()) {
        _transformChain = _modelInitTransform.getTraceToRoot();
    }
    Transform& modelTransform = _modelRootNode.get();

    if (_name == "GUI") {
        _modelRootNode.get().setScale({1.33333333f, 1.0f, 1.0f});
    }

    if (k && _name != "GUI") {
        t += 0.5 * delta;
        if (t > 1.0) {
            k = false;
            t = 0.0;
            transformInit();
        }

        if (_name == "嘴") {
            modelTransform.setScale(glm::mix(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3{1.5f, 1.5f, 10.0f}, curve.y(curve.reverse_x(t))));
        } else {
            modelTransform.setRotate(glm::mix(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, glm::radians(360.0), 0.0f}, curve.y(curve.reverse_x(t))));
        }
    }

    program.use();
    glBindVertexArray(vao);
    if (_name != "GUI") {
        program["Transform"].setMat4(projection * camera * Transform::worldMatrix(_transformChain));
    } else {
        program["Transform"].setMat4(projection * glm::translate(glm::mat4{1.0}, glm::vec3{0.0f, 0.0f, -1.0f}) * Transform::worldMatrix(_transformChain));
    }
    glDrawElements(GL_TRIANGLES, vi.size(), GL_UNSIGNED_INT, nullptr);
}
