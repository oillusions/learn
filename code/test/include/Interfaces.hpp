#pragma once

class Drawable {
    public:
        virtual ~Drawable() = default;
        virtual void init() = 0;
        virtual void render(double delta) = 0;
};

class Interactable {
    public:
        virtual ~Interactable() = default;

};

