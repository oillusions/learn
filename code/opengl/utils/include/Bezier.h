#pragma once
#include <glm/glm.hpp>

class Bezier {
    public:
        Bezier(const glm::vec2& start, const glm::vec2& control0, const glm::vec2& control1, const glm::vec2& end);
        Bezier(const glm::vec2& control0, const glm::vec2& control1);
        ~Bezier() = default;

        Bezier(const Bezier& other) = default;
        Bezier& operator = (const Bezier& other) = default;
        Bezier(Bezier&& other) noexcept = default;
        Bezier& operator = (Bezier&& other) noexcept = default;

        [[nodiscard]] double get_x(double t) const;
        [[nodiscard]] double get_y(double t) const;

        glm::vec2 get(double t) const;

        glm::vec2 operator [] (double t) const;
    private:
        glm::vec2 _start{};
        glm::vec2 _control0{};
        glm::vec2 _control1{};
        glm::vec2 _end{};
};