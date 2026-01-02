#pragma once
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>

class IResource {
    public:
        IResource() = default;
        IResource(const IResource&) = delete;
        IResource& operator = (const IResource&) = delete;
        IResource(IResource&&) = default;
        IResource& operator = (IResource&&) = default;
        virtual ~IResource() = default;
};

template<typename ResourceType>
class TypedResourceManager {
    static_assert(std::is_base_of_v<IResource, ResourceType>, "错误: 资源管理器模板类型[ ResourceType ]必须派生自 IResource");
    public:
        TypedResourceManager() = default;
        ~TypedResourceManager() = default;

        TypedResourceManager(const TypedResourceManager&) = delete;
        TypedResourceManager(TypedResourceManager&&) = default;
        TypedResourceManager& operator = (const TypedResourceManager&) = delete;
        TypedResourceManager& operator = (TypedResourceManager&&) = default;


        template<typename... Args>
        ResourceType& load(const std::string& identifier, Args&&... args) {
            resourceMap.emplace(identifier, std::make_unique<ResourceType>(std::forward<Args>(args)...));
            return *resourceMap.at(identifier);
        }

        ResourceType& find(const std::string& identifier) {
            return *resourceMap.at(identifier);
        }

        ResourceType& operator [] (const std::string& identifier) {
            return find(identifier);
        }

    protected:
        std::map<std::string, std::unique_ptr<ResourceType>> resourceMap;
};

class AnyResourceManager {
    public:
        AnyResourceManager() = default;
        ~AnyResourceManager() = default;

        AnyResourceManager(const AnyResourceManager&) = delete;
        AnyResourceManager(AnyResourceManager&&) = default;
        AnyResourceManager& operator = (const AnyResourceManager&) = delete;
        AnyResourceManager& operator = (AnyResourceManager&&) = default;

        template<typename ResourceType, typename... Args>
            ResourceType& load(const std::string& identifier, Args&&... args) {
            auto it = resourceMap[typeid(ResourceType)].emplace(identifier, std::make_unique<ResourceWrapper<ResourceType>>(std::forward<Args>(args)...));
            return find<ResourceType>(identifier);
        }

        template<typename ResourceType>
        ResourceType& find(const std::string& identifier) {
            auto type_it = resourceMap.find(typeid(ResourceType));
            if (type_it == resourceMap.end()) {
                glog.log<DefaultLevel::Error>("错误: 查找未知类型资源");
                throw;
            }
            auto identifier_it = type_it->second.find(identifier);
            if (identifier_it == type_it->second.end()) {
                glog.log<DefaultLevel::Error>("错误: 查找未知标识符资源");
                throw;
            }
            return *static_cast<ResourceType *>(dynamic_cast<ResourceWrapper<ResourceType> &>(*identifier_it->second).value());
        }

        template<typename ResourceType>
        ResourceType& operator [] (const std::string& identifier) {
            return find<ResourceType>(identifier);
        }

    protected:
        class BaseWrapper {
            public:
                virtual ~BaseWrapper() = default;
                virtual void* value() {
                    return nullptr;
                }
        };

        template<typename ResourceType>
        class ResourceWrapper: public BaseWrapper {
            public:
                template<typename ... Args>
                ResourceWrapper(Args... args) {
                    resource = std::make_unique<ResourceType>(std::forward<Args>(args)...);
                }
                ~ResourceWrapper() override = default;
                void* value() override {
                    return resource.get();
                }
            private:
                std::unique_ptr<ResourceType> resource;
        };

        std::map<std::type_index, std::map<std::string, std::unique_ptr<BaseWrapper>>> resourceMap;
};