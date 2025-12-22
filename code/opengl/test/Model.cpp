#include "Model.h"

#include "TestRenderCode.h"

using namespace std;
namespace fs = filesystem;

const fs::path vertexPath = "resource/shader/vertex.glsl";
const fs::path fragmentPath = "resource/shader/fragment.glsl";

Model::Model(const std::string &name, Node<Transform>::IndexWrapper modelTransformNode , VertexLayout<float> modelVertices):
    _name(name),
    vertexShader(Shader(Shader::Vertex, TestRenderCube::fileLoader(vertexPath))),
    fragmentShader(Shader(Shader::Fragment, TestRenderCube::fileLoader(fragmentPath))),
    _modelVertices(std::move(modelVertices)),
    _modelRootNode(modelTransformNode),
    _modelInitTransform(_modelRootNode.addChild("ModelInitTransform")){
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

    Transform& initTransform = _modelInitTransform.get();
    Transform& modelTransform = _modelRootNode.get();

    if (_name == "嘴") {
        initTransform.setTranslate({0.0f, 0.264522f, 0.303543f});
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
    }

    program.attach(vertexShader);
    program.attach(fragmentShader);
    (void) program.link();
}

void Model::render(double delta, const glm::mat4 &projection, const glm::mat4 &camera) {
    if (_transformChain.empty()) {
        _transformChain = _modelInitTransform.getTraceToRoot();
    }
    Transform& modelTransform = _modelRootNode.get();


    program.use();
    glBindVertexArray(vao);
    program["Transform"].setMat4(projection * camera * Transform::worldMatrix(_transformChain));
    glDrawElements(GL_TRIANGLES, vi.size(), GL_UNSIGNED_INT, nullptr);
}
