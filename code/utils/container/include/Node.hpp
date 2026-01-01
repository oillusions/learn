#pragma once
#include <algorithm>
#include <list>
#include <map>


template<typename CarriedType>
class Node {
    public:
        /**
         * @brief 根节点构造
         * @details 他似乎不需要详细注释[划掉]
         * @param identifier 节点标识符
         * @param value 预填充数据
         */
        explicit Node(const std::string& identifier, CarriedType value = {}):
                _identifier(identifier),
                _isRoot(true),
                _parent(nullptr),
                _depth(0),
                _value(std::move(value)),
                _childNodes(),
                _indexMap(){};

        /**
         * @brief 从当前节点下构造新节点
         * @details 他似乎不需要详细注释[划掉]
         * @param identifier 节点标识符
         * @param value 预填充数据
         * @return 节点引用
         */
        Node& addChild(const std::string& identifier, CarriedType value = {}) {
            if (identifier.empty()) return *this;
            _childNodes.push_back(std::move(Node(identifier, this, std::move(value))));
            _indexMap.emplace(identifier, _childNodes.size() - 1);
            return _childNodes.back();
        }

        /**
         * @brief 通过节点标识符查找并获取子节点引用
         * @details 他似乎不需要详细注释[划掉]
         * @param identifier 节点标识符
         * @return 节点引用
         * @note 若无法找到子节点, 将返回此节点本身
         */
        Node& getChild(const std::string& identifier) {
            if (identifier.empty()) return *this;
            auto it = _indexMap.find(identifier);
            if (it == _indexMap.end()) return *this;
            return _childNodes[it->second];
        }

        /**
         * @brief 从当前节点追溯到根节点
         * @details 他似乎不需要详细注释[划掉]
         * @param reverse 是否反转列表
         * @return 途径所有节点的引用数组
         * @note 若当前节点为根节点, 将返回空引用数组
         */
        std::vector<std::reference_wrapper<const CarriedType>> tracebackToRoot(bool reverse = false) const {
            if (_isRoot) return std::vector<std::reference_wrapper<const CarriedType>>{};

            std::vector<std::reference_wrapper<const CarriedType>> dataChian{};
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

        /**
         * @brief 获取携带值
         * @details 他似乎不需要详细注释[划掉]
         * @return 携带值引用
         */
        CarriedType& get() {
            return _value;
        }

        /**
         * @brief 通过类型转换操作符获取携带值
         * @details 他似乎不需要详细注释[划掉]
         * @return 携带值引用
         */
        operator CarriedType& () {
            return _value;
        }

        /**
         * @brief 通过下标操作符查找并获取子节点引用
         * @details 他似乎不需要详细注释[划掉]
         * @param identifier 节点标识符
         * @return 节点引用
         * @note 若无法找到子节点, 将返回此节点本身
         */
        Node& operator [] (const std::string& identifier) {
            return getChild(identifier);
        }


private:
        std::string _identifier;
        bool _isRoot{false};
        Node* _parent{nullptr};
        size_t _depth{0};
        CarriedType _value;
        std::list<Node> _childNodes;
        std::map<std::string, size_t> _indexMap;

        /**
         * @brief 子节点构造
         * @details 他似乎不需要详细注释[划掉]
         * @param identifier 节点标识符
         * @param parent 父节点指针
         * @param value 预填充值
         */
        Node(const std::string& identifier, Node* parent, CarriedType value):
            _identifier(identifier),
            _isRoot(false),
            _parent(parent),
            _depth(parent->_depth + 1),
            _value(std::move(value)),
            _childNodes(),
            _indexMap() {};
};
