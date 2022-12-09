#pragma once

#include <lore/lore.h>
#include <lore/math.h>

namespace lore {
namespace rt {

template<typename Float>
struct Ray {
    Vector3<Float> origin;
    Vector3<Float> direction;

    Ray() {}
    Ray(const Vector3<Float> &origin, const Vector3<Float> &direction)
    : origin(origin), direction(direction) {}

    Vector3<Float> operator()(Float t) const {
        return origin + t * direction;
    }
};

#ifndef __METAL__
template<typename Float>
std::ostream &operator<<(std::ostream &os, Ray<Float> const &ray) {
    os << "Ray { " << ray.origin << " -> " << ray.direction << " }";
    return os;
}
#endif

}
}
