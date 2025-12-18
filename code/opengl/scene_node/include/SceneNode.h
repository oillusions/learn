#pragma once
#include <string>
#include <vector>
#include <Transform.h>

class SceneNode {
    public:
        const std::string _name;
        Transform _localTransform;

        SceneNode(const SceneNode* parent, const std::string&, const Transform& transform);
        SceneNode(const std::string&, const Transform& transform);
        SceneNode(const SceneNode* parent, const std::string& name);
        SceneNode(const std::string& name);

        SceneNode(const SceneNode&) = delete;
        SceneNode& operator=(const SceneNode&) = delete;
        SceneNode(SceneNode&&) = default;
        SceneNode& operator=(SceneNode&&) = default;
        ~SceneNode() = default;


        SceneNode& addChild(const std::string& name, const Transform& transform);
        SceneNode& addChild(const std::string& name);

        bool isRoot() const;
        const SceneNode& getParent() const;

        Transform& getTransform();

        glm::mat4 getWorldMatrix() const;
        glm::mat4 getMatrix() const;

        SceneNode& getChild(const std::string& name);

        operator Transform&() {
            return _localTransform;
        }

    private:
        bool _isRoot{false};

        const SceneNode* _parent;
        std::vector<SceneNode> _childNodes;
};
