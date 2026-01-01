#pragma once
#include <string>

template<typename CommonType>
class Scene {
    public:
        class BaseCarryingType {
            public:
                virtual ~BaseCarryingType() = default;
                virtual CommonType& get() = 0;
                virtual operator CommonType& () = 0;
        };
        template<typename CarryingType>
        class SceneElement {
            static_assert(std::is_base_of_v<BaseCarryingType, CarryingType>, "错误: 模板类型必须为基承载类型派生");
            public:
            private:
                std::string _identifier;
                size_t _id{};
        };
};
