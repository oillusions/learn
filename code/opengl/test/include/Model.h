#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "Node.hpp"
#include "ShaderProgram.h"
#include "Transform.h"
#include "VertexLayout.hpp"

class Model {
    public:
        Model(const std::string& name, Node<Transform>::IndexWrapper modelTransformNode, VertexLayout<float> modelVertices);
        ~Model();

        Model(const Model& other) = delete;
        Model& operator = (const Model& other) = delete;

        Model(Model&& other) = default;
        Model& operator = (Model&& other) = default;

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
        Node<Transform>::IndexWrapper _modelRootNode;
        Node<Transform>::IndexWrapper _modelInitTransform;
        std::vector<std::reference_wrapper<Transform>> _transformChain;

};
