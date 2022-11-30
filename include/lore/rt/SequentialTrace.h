#include <lore/lore.h>
#include <lore/math.h>
#include <lore/rt/Ray.h>
#include <lore/lens/Lens.h>

namespace lore {
namespace rt {

template<typename Float>
struct SequentialTrace {
    Float wavelength;

    SequentialTrace(const Float &wavelength)
        : wavelength(wavelength) {}

    template<typename Intersector>
    bool operator()(Ray<Float> &ray, const Lens<Float> &lens, const Intersector &intersector) const {
        Float n1(1);

        bool firstSurface = true;
        for (const Surface<Float> &surface : lens.surfaces) {
            if (firstSurface) {
                ray.origin.z() -= surface.thickness;
                n1 = surface.ior(wavelength);
                firstSurface = false;
                continue;
            }

            Float t;
            if (!intersector(ray, surface, t)) {
                return false;
            }

            ray.origin = ray(t);
            if (surface.checkAperture) {
                const Float rSqr = sqr(ray.origin.x()) + sqr(ray.origin.y());
                if (rSqr > sqr(surface.aperture)) {
                    // hit checked aperture stop
                    return false;
                }
            }

            const Vector3<Float> normal = surface.radius == 0 ?
                                          Vector3<Float>(0, 0, -copysign(Float(1), ray.direction.z())) :
                                          -faceforward(
                                              (ray.origin - Vector3<Float>(0, 0, surface.radius)).normalized(),
                                              ray.direction
                                          );
            const Float n2 = surface.ior(wavelength);

            if (!refract(ray.direction, normal, ray.direction, n1 / n2)) {
                return false;
            }

            ray.origin.z() -= surface.thickness;
            n1 = n2;
        }

        return true;
    }
};

}
}
