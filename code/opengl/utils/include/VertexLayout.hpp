#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <glad/glad.h>

template<typename T>
class VertexLayout {
    public:

        class LayoutElement {
            public:
                std::vector<T> _source;
                std::string identifier;
                size_t length;
                size_t location{};
                size_t origin{};
                size_t step{};
                bool* _isDirty{};


                LayoutElement(const std::string& identifier, size_t length, size_t origin, size_t location):
                    identifier(identifier),
                    length(length),
                    location(location),
                    origin(origin * sizeof(T)) {}

                LayoutElement(const std::string& identifier, size_t length): LayoutElement(identifier, length, 0, 0) {}

                ~LayoutElement() = default;

                void dirty() const {
                    if (_isDirty != nullptr) {
                        *_isDirty = true;
                    }
                }

                void setSource(const std::vector<T>& source) {
                    if (_source == source) return;
                    _source = source;
                    dirty();
                }

                void setSource(std::vector<T>&& source) {
                    if (_source == source) return;
                    _source = std::move(source);
                    dirty();
                }

                const std::vector<T>& getSource() {
                    return _source;
                }

                operator const std::vector<T>& () {
                    return getSource();
                }

                LayoutElement& operator = (const std::vector<T>& source) {
                    setSource(source);
                    return *this;
                }

                const float& operator [] (size_t index) const {
                    return _source[index];
                }

            };

        class LayoutBuilder {
            public:
                LayoutBuilder() {
                    elements = std::vector<LayoutElement>();
                    identifierMap = std::map<std::string, size_t>();
                }
                ~LayoutBuilder() = default;
                LayoutBuilder& elementNumber(size_t number) {
                    if (number != 0) {
                        elements.resize(number);
                    }
                    return *this;
                }

                LayoutBuilder& appendElement(const std::string& identifier, size_t length) {
                    if (identifier.empty()) {
                        throw std::runtime_error("identifier为空： " + identifier);
                    }
                    if (length == 0) {
                        throw std::runtime_error("元素长度为空: " + std::to_string(length));
                    }
                    elements.emplace_back(LayoutElement(identifier, length, originCounter, locationCounter));
                    identifierMap.emplace(identifier, locationCounter);
                    locationCounter++;
                    originCounter += length;
                    return *this;
                }

                LayoutBuilder& attachSource(const std::string& identifier, const std::vector<T>& source) {
                    if (elements.empty()) {
                        std::cerr << "当前未拥有任何元素" << std::endl;
                        return *this;
                    }
                    auto index = identifierMap.find(identifier);
                    if (index == identifierMap.end()) {
                        std::cerr << "当前未拥有 " << identifier << " 标识符的元素" << std::endl;
                        return *this;
                    }
                    elements.at(index->second).setSource(source);
                    return *this;
                }

                LayoutBuilder& attachSource(const std::string& identifier, std::vector<T>&& source) {
                    if (elements.empty()) {
                        std::cerr << "当前未拥有任何元素" << std::endl;
                        return *this;
                    }
                    auto index = identifierMap.find(identifier);
                    if (index == identifierMap.end()) {
                        std::cerr << "当前未拥有 " << identifier << " 标识符的元素" << std::endl;
                        return *this;
                    }
                    elements.at(index->second).setSource(std::move(source));
                    return *this;
                }

                LayoutBuilder& attachIndices(const std::vector<unsigned int>& indices) {
                    rawIndices = indices;
                    return *this;
                }

                LayoutBuilder& attachIndices(std::vector<unsigned int>&& indices) {
                    rawIndices = std::move(indices);
                    return *this;
                }

                bool contain(const std::string& identifier) {
                    if (elements.empty()) return false;
                    if (identifierMap.find(identifier) == identifierMap.end()) return false;
                    return true;
                }

                VertexLayout build() {
                    if (elements.empty()) {
                        throw std::runtime_error("构建时布局元素列表为空");
                        return VertexLayout(std::move(elements));
                    }
                    for (auto& e : elements) {
                        e.step = originCounter * sizeof(T);
                    }
                    return VertexLayout(std::move(elements), std::move(identifierMap), std::move(rawIndices));
                }

            private:
                std::vector<LayoutElement> elements;
                std::map<std::string, size_t> identifierMap;
                std::vector<unsigned int> rawIndices;
                size_t locationCounter{0};
                size_t stepCounter{0};
                size_t originCounter{0};
        };

        static LayoutBuilder builder() {
            return LayoutBuilder();
        }

        explicit VertexLayout(std::vector<LayoutElement> layout):
                VertexLayout(std::move(layout), std::move(std::map<std::string, size_t>())) {}

        VertexLayout(const VertexLayout&) = delete;
        VertexLayout& operator=(const VertexLayout&) = delete;

        VertexLayout(VertexLayout&& other) noexcept :
            _layout(std::move(other._layout)),
            _cache(std::move(other._cache)),
            _indices(std::move(other._indices)),
            _rawIndices(std::move(other._rawIndices)),
            _identifierMap(std::move(other._identifierMap)),
            _isDirty(true)
            {
                size_t index{0};
                for (LayoutElement& e : _layout) {
                    e._isDirty = &this->_isDirty;
                    _identifierMap.emplace(e.identifier, index);
                    index++;
                }
            };
        VertexLayout& operator=(VertexLayout&& other)  noexcept {
            if (this != &other) {
                _layout = std::move(other._layout);
                _cache = std::move(other._cache);
                _indices = std::move(other._indices);
                _rawIndices = std::move(other._rawIndices);
                _identifierMap = std::move(other._identifierMap);
                _isDirty = true;
                for (LayoutElement& e : _layout) {
                    e._isDirty = &this->_isDirty;
                }
                _isDirty = true;
            }
            return *this;
        }

        ~VertexLayout() = default;

        void test() {
            for (const auto& e : _layout) {
                std::cout << "identifier: " << e.identifier << ", location: " << e.location << ",length: " << e.length << ", origin: " << e.origin << ", step:" << e.step << std::endl;
            }
        }

        void bufferLayoutDeclaration() {
            if (_layout.empty()) {
                throw std::runtime_error("当前未拥有任何元素");
            }
            GLenum type = GL_FLOAT;
            if constexpr (std::is_same<T, float>()) {
                type = GL_FLOAT;
            }

            for (const auto& e : _layout) {
                glVertexAttribPointer(e.location, e.length, type, GL_FALSE, e.step, reinterpret_cast<void*>(e.origin));
                glEnableVertexAttribArray(e.location);
            }
        }

        const std::vector<T>& assemblyBuffer() {
            if (!_isDirty) {
                return _cache;
            }
            size_t size{};
            for (const auto& e : _layout) {
                size += e._source.size();
            }
            _cache.clear();
            _cache.reserve(size);
            _indices.clear();
            _indices.reserve(size);

            std::cout << "size: " << size << std::endl;
            size_t stepCounter{};
            size_t indexCounter{};
            size_t i{};
            while (true) {
                for (auto& e : _layout) {
                    _cache.insert(_cache.end(), e.length, 0);
                    std::copy_n(e._source.begin() + (e.length * stepCounter),
                    e.length,
                    _cache.begin() + i
                    );
                    i += e.length;
                }
                stepCounter++;
                _indices.push_back(indexCounter);
                indexCounter++;
                if (i >= size) break;
            }

            std::cout << "out: " << std::endl;
            _isDirty = false;
            return _cache;
        }

        void indices(const std::vector<unsigned int>& indexStr) {
            if (!indexStr.empty()) {
                _rawIndices = indexStr;
            }
        }

        void indices(std::vector<unsigned int>&& indexStr) {
            if (!indexStr.empty()) {
                _rawIndices = std::move(indexStr);
            }
        }


        const std::vector<T>& ExpandIndices() {
            if (!_isDirty) {
                return _cache;
            }
            if (_rawIndices.empty()) {
                std::cerr << "错误: 索引为空" << std::endl;
                return _cache;
            }

            if (_rawIndices.empty()) {
                std::cerr << "错误: 顶点索引为空" << std::endl;
                return _cache;
            }
            size_t size = (_rawIndices.size() / _layout.size()) * (_layout[0].step / sizeof(T));
            _cache.clear();
            _cache.reserve(size);
            _indices.clear();
            _indices.reserve(_rawIndices.size() / (_layout[0].step / sizeof(T)));

            size_t i{};
            size_t stepCounter{0};
            size_t elementIndex{0};
            size_t indexCounter{0};
            for (const auto& index : _rawIndices) {
                auto& e = _layout.at(elementIndex);
                _cache.insert(_cache.end(), e.length, 0);
                std::copy_n(e._source.begin() + (e.length * index),
                    e.length,
                    _cache.begin() + i
                    );
                i += e.length;
                stepCounter++;
                elementIndex++;
                elementIndex %= _layout.size();
                _indices.push_back(indexCounter);
                indexCounter++;
            }
            _isDirty = false;
            return _cache;
        }

        const std::vector<unsigned int>& bufferOfIndices() {
            return _indices;
        }

        bool contain(const std::string& identifier) {
            if (_layout.empty()) return false;
            if (_identifierMap.find(identifier) == _identifierMap.end()) return false;
            return true;
        }

        LayoutElement& operator [] (const std::string& identifier) {
            if (_layout.empty()) {
                throw std::runtime_error("当前未拥有任何元素");
            }
            auto index = _identifierMap.find(identifier);
            if (index == _identifierMap.end()) {
                throw std::runtime_error("当前未拥有 " + identifier + " 标识符的元素");
            }
            return _layout.at(index->second);
        }


    private:
        std::vector<LayoutElement> _layout;
        std::map<std::string, size_t> _identifierMap;
        std::vector<T> _cache;
        std::vector<unsigned int> _indices;
        std::vector<unsigned int> _rawIndices;
        bool _isDirty{true};

        explicit VertexLayout(std::vector<LayoutElement> layout, std::map<std::string, size_t> identifierMap, std::vector<unsigned int> rawIndices):
            _layout(std::move(layout)),
            _identifierMap(std::move(identifierMap)),
            _cache(std::vector<T>()),
            _indices(std::vector<unsigned int>()),
            _rawIndices(std::move(rawIndices)),
            _isDirty(true) {
            size_t index{0};
            for (LayoutElement& e : _layout) {
                e._isDirty = &this->_isDirty;
                _identifierMap.emplace(e.identifier, index);
                index++;
            }
        }

        explicit VertexLayout(std::vector<LayoutElement> layout, std::map<std::string, size_t> identifierMap):
            VertexLayout(std::move(layout), std::move(identifierMap), std::move(std::vector<unsigned int>())) {};
};