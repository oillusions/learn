#pragma once
#include <EventBus.hpp>
#include <Model.h>

class TestRenderUI {
    public:
        TestRenderUI(EventBus ebus):
            _ebus(ebus) {};
        ~TestRenderUI() = default;

        Mode& init();

    private:
        EventBus& _ebus;
};
