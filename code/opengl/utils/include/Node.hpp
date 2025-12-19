#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

template<typename T>
class Node {
    public:
    explicit Node(const std::string& identifier):
            _isRoot(true),
            _identifier(identifier),
            _parent(this),
            _depth(0),
            _data(),
            _childs(),
            _identifierMap() {}

        Node(Node&& other) = default;
        Node& operator = (Node&& other) = default;

        Node(const Node& other) = delete;
        Node& operator = (const Node& other) = default;

        Node& addChild(const std::string& identifier, const T& data) {
            _identifierMap.emplace(identifier, _childs.size());
            _childs.push_back(Node(identifier, this, data));
            return _childs.back();
        }

        Node& addChild(const std::string& identifier) {
            _identifierMap.emplace(identifier, _childs.size());
            _childs.push_back(Node(identifier, this));
            return _childs.back();
        }

        std::vector<std::reference_wrapper<const T>> getTraceToRoot(bool reverse = false) const {
            if (_isRoot) return std::move(std::vector<std::reference_wrapper<const T>>());

            std::vector<std::reference_wrapper<const T>> dataset{};
            dataset.reserve(_depth + 1);
            dataset.emplace_back(_data);

            const Node* parent = _parent;
            for (size_t i = 0; i < _depth; i++) {
                dataset.emplace_back(parent->_data);
                parent = parent->_parent;
            }
            if (reverse) {
                std::reverse(dataset.begin(), dataset.end());
            }
            return dataset;
        }

        std::vector<std::reference_wrapper<T>> getTraceToRoot(const bool reverse = false) {
            if (_isRoot) return std::move(std::vector<std::reference_wrapper<T>>());

            std::vector<std::reference_wrapper<T>> dataset{};
            dataset.reserve(_depth + 1);
            dataset.emplace_back(_data);

            Node* parent = _parent;
            for (size_t i = 0; i < _depth; i++) {
                dataset.emplace_back(parent->_data);
                parent = parent->_parent;
            }
            if (reverse) {
                std::reverse(dataset.begin(), dataset.end());
            }
            return dataset;
        }

        T& get() {
            return _data;
        }

        const T& get() const {
            return _data;
        }

        const std::string& getIdentifier() {
            return _identifier;
        }

        bool isRoot() const {
            return _isRoot;
        }

        Node& operator [] (const std::string& identifier) {
            if (identifier.empty()) return *this;
            auto index = _identifierMap.find(identifier);
            if (index == _identifierMap.end()) return *this;
            return _childs.at(_identifierMap[identifier]);
        }

        const Node& operator[](const std::string& identifier) const {
            if (identifier.empty()) return *this;
            auto index = _identifierMap.find(identifier);
            if (index == _identifierMap.end()) return *this;
            return _childs.at(_identifierMap.at(identifier));
        }

        operator T& () {
            return _data;
        }

        operator const T& () const {
            return _data;
        }

        operator const std::string& () const {
            return _identifier;
        }


    private:
        bool _isRoot{};
        std::string _identifier;
        Node* _parent;
        size_t _depth;
        T _data;
        std::vector<Node> _childs;
        std::map<std::string, size_t> _identifierMap;

        Node(const std::string& identifier, Node* parent, const T& data):
            _isRoot(false),
            _identifier(identifier),
            _parent(parent),
            _depth(parent->_depth + 1),
            _data(data),
            _childs(),
            _identifierMap() {};

        Node(const std::string& identifier, Node* parent):
            Node(identifier, parent, T()) {}
};
