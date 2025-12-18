#include "SceneNode.h"

SceneNode::SceneNode(const SceneNode* parent, const std::string& name, const Transform &transform):
                    _parent(parent),
                    _name(name),
                    _localTransform(transform){};

SceneNode::SceneNode(const std::string& name, const Transform &transform):
                    _parent(nullptr),
                    _name(name),
                    _localTransform(transform),
                    _isRoot(true) {};

SceneNode::SceneNode(const SceneNode* parent, const std::string& name): SceneNode(parent, name, Transform()) {}

SceneNode::SceneNode(const std::string& name): SceneNode(name, Transform()) {}


SceneNode &SceneNode::addChild(const std::string &name, const Transform &transform) {
    return _childNodes.emplace_back(this, name, transform);
}

SceneNode &SceneNode::addChild(const std::string &name) {
    return addChild(name, Transform());
}

Transform &SceneNode::getTransform() {
    return _localTransform;
}

const SceneNode &SceneNode::getParent() const {
    return *_parent;
}

glm::mat4 SceneNode::getMatrix() const {
    return _localTransform.getMatrix();
}

glm::mat4 SceneNode::getWorldMatrix() const {
    if (isRoot()) {
        return _localTransform.getMatrix();
    }
    return _parent->getWorldMatrix() * _localTransform.getMatrix();
}

bool SceneNode::isRoot() const {
    return _isRoot;
}

SceneNode& SceneNode::getChild(const std::string &name) {
    for (auto& node : _childNodes) {
        if (node._name == name) {
            return node;
        }
    }
    return *this;
}









