#include <lore/lore.h>
#include <lore/math.h>
#include <lore/rt/Ray.h>
#include <lore/lens/Surface.h>

namespace lore {
namespace rt {

template<typename Float>
struct GeometricalIntersector {
    bool operator()(const Ray<Float> &ray, const Surface<Float> &surface, Float &t) const {
        if (surface.radius == 0) {
            if (ray.direction.z() == 0) {
                return false;
            }

            t = -ray.origin.z() / ray.direction.z();
            return true;
        }

        const Float a = ray.direction.z() * surface.radius - ray.origin.dot(ray.direction); // p/2
        const Float b = ray.origin.lengthSquared() - Float(2) * ray.origin.z() * surface.radius; // q
        if (b == 0) {
            t = 0;
            return true;
        }

        const Float disc = sqr(a) - b;
        if (disc < 0) {
            return false;
        }

        const Float rad = sqrt(disc);
        //t = a - rad;
        t = b / (a + sqrt(disc)); // @todo this seems to be more numerically stable, but needs verification
        if (t < 0) {
            //t = a + rad;
            t = b / (a - sqrt(disc));
        }
        if (t < 0) {
            return false;
        }

        return true;
    }
};

}
}
