#pragma once
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "Node.hpp"
#include "ShaderProgram.h"
#include "Transform.h"
#include "VertexLayout.hpp"

class Model {
    public:
        Model(const std::string& name, Node<Transform>& modelTransformNode, VertexLayout<float> modelVertices);
        ~Model();

        Model(const Model& other) = delete;
        Model& operator = (const Model& other) = delete;

        Model(Model&& other) noexcept = default;
        Model& operator = (Model&& other) noexcept = delete;

        void init();
        void render(double delta, const glm::mat4& projection, const glm::mat4& camera);
    private:
        std::string _name;
        unsigned int vao{};
        unsigned int vbo{};
        unsigned int ebo{};
        Shader vertexShader;
        Shader fragmentShader;
        ShaderProgram program;
        VertexLayout<float> _modelVertices;
        std::vector<float> vv;
        std::vector<unsigned int> vi;
        Node<Transform>& _modelRootNode;
        Node<Transform>& _modelInitTransform;

};
