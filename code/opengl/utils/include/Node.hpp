#pragma once
#include <algorithm>
#include <list>
#include <map>


template<typename T>
class Node {
    public:
        explicit Node(const std::string& identifier, T value = {}):
            _identifier(identifier),
            _isRoot(true),
            _parent(nullptr),
            _depth(0),
            _value(std::move(value)),
            _childNodes(),
            _indexMap(){};

        Node& addChild(const std::string& identifier, T value = {}) {
            if (identifier.empty()) return *this;
            _childNodes.push_back(std::move(Node(identifier, this, std::move(value))));
            _indexMap.emplace(identifier, _childNodes.size() - 1);
            return _childNodes.back();
        }

        Node& getChild(const std::string& identifier) {
            if (identifier.empty()) return *this;
            auto it = _indexMap.find(identifier);
            if (it == _indexMap.end()) return *this;
            return _childNodes[it->second];
        }

        std::vector<std::reference_wrapper<const T>> tracebackToRoot(bool reverse = false) const {
            if (_isRoot) return std::vector<std::reference_wrapper<const T>>{};

            std::vector<std::reference_wrapper<const T>> dataChian{};
            dataChian.reserve(_depth + 1);
            dataChian.emplace_back(_value);
            Node* parent = _parent;
            for (size_t i = 0; i < _depth; i++) {
                dataChian.emplace_back(parent->_value);
                parent = parent->_parent;
            }

            if (reverse) {
                std::reverse(dataChian.begin(), dataChian.end());
            }
            return dataChian;
        }

        T& get() {
            return _value;
        }

        operator T& () {
            return _value;
        }

        Node& operator [] (const std::string& identifier) {
            return getChild(identifier);
        }


private:
        std::string _identifier;
        bool _isRoot{false};
        Node* _parent{nullptr};
        size_t _depth{0};
        T _value;
        std::list<Node> _childNodes;
        std::map<std::string, size_t> _indexMap;

        Node(const std::string& identifier, Node* parent, T value):
            _identifier(identifier),
            _isRoot(false),
            _parent(parent),
            _depth(parent->_depth + 1),
            _value(std::move(value)),
            _childNodes(),
            _indexMap() {};
};
