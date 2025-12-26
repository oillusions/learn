#pragma once
#include <iostream>
#include <typeindex>
#include <functional>
#include <memory>
#include <map>

class EventBus {
    public:
        EventBus() = default;
        ~EventBus() = default;

        template<typename EventType>
        void publish(const std::string& identifier, EventType& content) const {
            if (identifier.empty()) return;
            if (_subscriptionMap.empty()) return;

            if (const auto typeMap_it = _subscriptionMap.find(typeid(EventType)); typeMap_it != _subscriptionMap.end()) {
                if (typeMap_it->second.empty()) return;
                if (const auto identifierMap_it = typeMap_it->second.find(identifier); identifierMap_it != typeMap_it->second.end()) {
                    if (identifierMap_it->second.empty()) return;
                    for (auto& call : identifierMap_it->second) {
                        if (auto* wrapper = dynamic_cast<CallWrapper<EventType&>*>(call.get())) {
                            wrapper->call(content);
                        }
                    }
                }
            }
        }

        template<typename EventType>
        void subscribe(const std::string& identifier, std::function<void(EventType&)> call) const {
            _subscriptionMap[typeid(EventType)][identifier].emplace_back(std::make_unique<CallWrapper<EventType&>>(call));
        }

    private:
        class CallBase {
            public:
                virtual ~CallBase() = default;
                virtual void call() {
                    std::cout << "空" << std::endl;
                }
            };

        template<typename T>
        class CallWrapper: public CallBase{
            public:
                CallWrapper(std::function<void(T&)> call): _call(call) {}
                ~CallWrapper() override = default;

                void call(T& content) {
                    _call(content);
                }

                void operator () (T& content) {
                    call(content);
                }
            private:
                std::function<void(T&)> _call;
        };

        mutable std::map<std::type_index, std::map<std::string, std::vector<std::unique_ptr<CallBase>>>> _subscriptionMap;

};
