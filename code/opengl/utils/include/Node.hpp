#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

template<typename T>
class Node {
    public:
        class IndexWrapper {
            public:
                IndexWrapper(Node& essence):
                    IndexWrapper(essence._root, essence._localIndex) {};
                ~IndexWrapper() = default;

                IndexWrapper(const IndexWrapper& other) = default;
                IndexWrapper& operator = (const IndexWrapper& other) = default;

                IndexWrapper(IndexWrapper&& other) = default;
                IndexWrapper& operator = (IndexWrapper&& other) = default;


                IndexWrapper addChild(const std::string& identifier, const T& data) {
                    if (_index == static_cast<size_t>(-1)) return _root->addChild(identifier, data);
                    return _root->_nodes->at(_index).addChild(identifier, data);
                }

                IndexWrapper addChild(const std::string& identifier) {
                    if (_index == static_cast<size_t>(-1)) return _root->addChild(identifier);
                    return _root->_nodes->at(_index).addChild(identifier);
                }

                std::vector<std::reference_wrapper<const T>> getTraceToRoot(bool reverse = false) const {
                    if (_index == static_cast<size_t>(-1)) return _root->getTraceToRoot(reverse);
                    return _root->_nodes->at(_index).getTraceToRoot(reverse);
                }

                std::vector<std::reference_wrapper<T>> getTraceToRoot(bool reverse = false) {
                    if (_index == static_cast<size_t>(-1)) return _root->getTraceToRoot(reverse);
                    return _root->_nodes->at(_index).getTraceToRoot(reverse);
                }

                const std::string& getIdentifier() const {
                    if (_index == static_cast<size_t>(-1)) return _root->_identifier;
                    return _root->_nodes->at(_index)._identifier;
                }

                operator const std::string& () const {
                    return getIdentifier();
                }

                IndexWrapper operator [] (const std::string& identifier) {
                    if (_index == static_cast<size_t>(-1)) return _root->operator[](identifier);
                    return _root->_nodes->at(_index).operator[](identifier);
                }

                IndexWrapper operator [](const std::string &identifier) const {
                    if (_index == static_cast<size_t>(-1)) return _root->operator[](identifier);
                    return _root->_nodes->at(_index).operator[](identifier);
                }

                bool isRoot() const {
                    if (_index == static_cast<size_t>(-1)) return _root->_isRoot;
                    return _root->_nodes->at(_index)._isRoot;
                }

                T& get() {
                    if (_index == static_cast<size_t>(-1)) return *_root->_data;
                    return *_root->_nodes->at(_index)._data;
                }

                const T& get() const {
                    if (_index == static_cast<size_t>(-1)) return *_root->_data;
                    return *_root->_nodes->at(_index)._data;
                }

                size_t index() {
                    return _index;
                }

                operator T& () {
                    return get();
                }

                operator const T& () const {
                    return get();
                }

                operator Node& () {
                    if (_index == static_cast<size_t>(-1)) return *_root;
                    return _root->_nodes->at(_index);
                }

                operator const Node& () const {
                    if (_index == static_cast<size_t>(-1)) return *_root;
                    return _root->_nodes->at(_index);
                }

                Node* operator -> () {
                    if (_index == static_cast<size_t>(-1)) return _root;
                    return &_root->_nodes->at(_index);
                }

                const Node* operator -> () const {
                    if (_index == static_cast<size_t>(-1)) return _root;
                    return &_root->_nodes->at(_index);
                }

            private:
                IndexWrapper(size_t index, Node* root):
                    _index(index),
                    _root(root) {}
                size_t _index;
                Node* _root;
                friend class Node;
        };

        explicit Node(const std::string& identifier):
            _isRoot(true),
            _identifier(identifier),
            _root(this),
            _depth(0),
            _localIndex(static_cast<size_t>(-1)),
            _parentIndex(static_cast<size_t>(-1)),
            _data(std::make_unique<T>()),
            _nodes(std::make_unique<std::vector<Node>>()) {}

        Node(Node&& other) = default;
        Node& operator = (Node&& other) = default;

        Node(const Node& other) = delete;
        Node& operator = (const Node& other) = delete;

        IndexWrapper addChild(const std::string& identifier, const T& data = T()) {
            if (identifier.empty()) return {_localIndex, _root};
            if (_isRoot) {
                _nodes->push_back(Node(identifier, *this, _root->_nodes->size(), data));
                _childs.emplace(identifier, _nodes->size() - 1);
                return {_nodes->size() - 1, this};
            }
            return _root->addChild(identifier, _localIndex, data);
        }

        std::vector<std::reference_wrapper<const T>> getTraceToRoot(bool reverse = false) const {
            if (_isRoot) return std::move(std::vector<std::reference_wrapper<const T>>());

            std::vector<std::reference_wrapper<const T>> dataset{};
            dataset.reserve(_depth + 1);
            dataset.emplace_back(*_data);

            const IndexWrapper parent = {_parentIndex, _root};
            for (size_t i = 0; i < _depth; i++) {
                dataset.emplace_back(*static_cast<Node&>(parent)._data);
                parent = {static_cast<Node&>(parent)._parentIndex, _root};
                if (static_cast<Node&>(parent)._parentIndex == static_cast<size_t>(-1)) {
                    dataset.emplace_back(*_root->_data);
                    break;
                }
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
            dataset.emplace_back(*_data);

            IndexWrapper parent = {_parentIndex, _root};
            for (size_t i = 0; i < _depth; i++) {
                dataset.emplace_back(*static_cast<Node&>(parent)._data);
                parent = {static_cast<Node&>(parent)._parentIndex, _root};

                if (static_cast<Node&>(parent)._parentIndex == static_cast<size_t>(-1)) {
                    dataset.emplace_back(*_root->_data);
                    break;
                }
            }
            if (reverse) {
                std::reverse(dataset.begin(), dataset.end());
            }
            return dataset;
        }

        T& get() {
            return *_data;
        }

        const T& get() const {
            return *_data;
        }

        const std::string& getIdentifier() const {
            return _identifier;
        }

        bool isRoot() const {
            return _isRoot;
        }

        IndexWrapper operator [] (const std::string& identifier) {
            if (identifier.empty()) return *this;
            auto index = _childs.find(identifier);
            if (index == _childs.end()) return *this;
            return {index->second, _root};
        }

        IndexWrapper operator[](const std::string &identifier) const {
            if (identifier.empty()) return *this;
            auto index = _childs.find(identifier);
            if (index == _childs.end()) return *this;
            return {index->second, _root};
        }

        operator T& () {
            return *_data;
        }

        operator const T& () const {
            return *_data;
        }

        operator const std::string& () const {
            return _identifier;
        }

        operator IndexWrapper () {
            return IndexWrapper(_localIndex, _root);
        }

        operator const IndexWrapper () const {
            return IndexWrapper(_localIndex, _root);
        }


    private:
        bool _isRoot{};
        std::string _identifier;
        Node* _root;
        size_t _depth;
        size_t _localIndex;
        size_t _parentIndex;
        std::unique_ptr<T> _data;
        std::unique_ptr<std::vector<Node>> _nodes;
        std::map<std::string, size_t> _childs;

        Node(const std::string& identifier, const Node& parent, size_t index, const T& data):
            _identifier(identifier),
            _root(parent._root),
            _depth(parent._depth + 1),
            _localIndex(index),
            _parentIndex(parent._localIndex),
            _data(std::make_unique<T>(data)),
            _nodes(nullptr){};

        Node(const std::string& identifier, const Node& parent, size_t index):
            Node(identifier, parent, index, T{}) {}


        IndexWrapper addChild(const std::string& identifier, size_t index, const T& data = T()) {
            if (identifier.empty()) return {_localIndex, _root};
            if (_isRoot) {
                _root->_nodes->push_back(Node(identifier, _root->_nodes->at(index), _root->_nodes->size(), data));
                _root->_nodes->at(index)._childs.emplace(identifier, _root->_nodes->size() - 1);
                return {_root->_nodes->size() - 1, _root};
            }


            return _root->addChild(identifier, _localIndex, data);
        }
};
