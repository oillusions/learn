#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct FrameSize_Event {
    GLFWwindow* window{};
    int width;
    int height;
};

struct Keyboard_Event {
    GLFWwindow* window{};
    int key;
    int scancode;
    int action;
    int mods;
};

struct MouseMove_Event {
    GLFWwindow* window{};
    double x;
    double y;
};

struct MouseButton_Event {
    int button;
    int action;
    int mods;
};

struct MouseScroll_Event {
    double x_offset;
    double y_offset;
};