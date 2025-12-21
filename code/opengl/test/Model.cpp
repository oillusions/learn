#include "Model.h"

#include "TestRenderCode.h"

using namespace std;
namespace fs = filesystem;

const fs::path vertexPath = "resource/shader/vertex.glsl";
const fs::path fragmentPath = "resource/shader/fragment.glsl";

Model::Model(const std::string &name, Node<Transform>& modelTransformNode , VertexLayout<float> modelVertices):
    _name(name),
    vertexShader(Shader(Shader::Vertex, TestRenderCube::fileLoader(vertexPath))),
    fragmentShader(Shader(Shader::Fragment, TestRenderCube::fileLoader(fragmentPath))),
    _modelVertices(std::move(modelVertices)),
    _modelRootNode(modelTransformNode),
    _modelInitTransform(_modelRootNode.addChild("ModelInitTransform")){
}

Model::Model(Model &&other) noexcept:
    _{

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

    program.attach(vertexShader);
    program.attach(fragmentShader);
    (void) program.link();
}

void Model::render(double delta, const glm::mat4 &projection, const glm::mat4 &camera) {
    static auto dataset = _modelInitTransform.getTraceToRoot(true);

    program.use();
    glBindVertexArray(vao);
    program["Transform"].setMat4(projection * camera * Transform::worldMatrix(dataset));
    glDrawElements(GL_TRIANGLES, vi.size(), GL_UNSIGNED_INT, nullptr);
}
