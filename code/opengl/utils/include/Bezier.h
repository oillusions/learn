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

        [[nodiscard]] double x(double t) const;
        [[nodiscard]] double y(double t) const;

        [[nodiscard]] double derivative_x(double t) const;
        [[nodiscard]] double derivative_y(double t) const;

        [[nodiscard]] double reverse_x(double t, size_t accuracy = 3) const;
        [[nodiscard]] double reverse_y(double t, size_t accuracy = 3) const;

        [[nodiscard]] glm::vec2 get(double t) const;

        glm::vec2 operator [] (double t) const;
    private:
        glm::vec2 _start{};
        glm::vec2 _control0{};
        glm::vec2 _control1{};
        glm::vec2 _end{};
};