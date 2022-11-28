#include <lore/lore.h>
#include <lore/math.h>
#include <lore/lens/Surface.h>

namespace lore::rt {

template<typename Float>
struct Ray {
    Vector3<Float> origin;
    Vector3<Float> direction;

    Vector3<Float> operator()(Float t) const {
        return origin + t * direction;
    }
};

template<typename Float>
bool geometrical(const Ray<Float> &ray, const Surface<Float> &surface, Float &t) {
    if (surface.radius == 0) {
        if (ray.direction.z() == 0) {
            return false;
        }

        t = -ray.origin.z() / ray.direction.z();
        return true;
    }

    const Float a = ray.direction.z() * surface.radius - ray.origin.dot(ray.direction);
    const Float b = ray.origin.lengthSquared() - 2 * ray.origin.z() * surface.radius;
    const Float disc = sqr(a) - b;
    if (disc < 0) {
        return false;
    }

    const Float rad = sqrt(disc);
    t = a - rad;
    if (t < 0) {
        t = a + rad;
    }

    return true;
}

}
