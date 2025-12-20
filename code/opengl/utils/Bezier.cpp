#include "Bezier.h"

#include <glm/ext/quaternion_common.hpp>

Bezier::Bezier(const glm::vec2 &start, const glm::vec2 &control0, const glm::vec2 &control1, const glm::vec2 &end):
    _start(start),
    _control0(control0),
    _control1(control1),
    _end(end) {
}

Bezier::Bezier(const glm::vec2 &control0, const glm::vec2 &control1):
    Bezier({0.0, 0.0}, control0, control1, {1.0, 1.0}){
}

double Bezier::get_x(double t) const {
    if (t <= 0.0) return _start.x;
    if (t >= 1.0) return _end.x;

    const double t2 = t * t;
    const double t3 = t2 * t;
    const double mt = 1.0 - t;
    const double mt2 = mt * mt;
    const double mt3 = mt2 * mt;

    return mt3 * _start.x
        + 3.0 * mt2 * t *_control0.x
        + 3.0 * mt * t2 *_control1.x
        + t3 *_end.x;
}

double Bezier::get_y(double t) const {
    if (t <= 0.0) return _start.y;
    if (t >= 1.0) return _end.y;

    const double t2 = t * t;
    const double t3 = t2 * t;
    const double mt = 1.0 - t;
    const double mt2 = mt * mt;
    const double mt3 = mt2 * mt;

    return mt3 * _start.y
        + 3.0 * mt2 * t *_control0.y
        + 3.0 * mt * t2 *_control1.y
        + t3 *_end.y;
}

glm::vec2 Bezier::get(double t) const {
    return {get_x(t), get_y(t)};
}

glm::vec2 Bezier::operator[](double t) const {
    return {get_x(t), get_y(t)};
}
