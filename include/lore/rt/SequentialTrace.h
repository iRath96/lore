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
        for (const Surface<Float> &surface : lens.surfaces) {
            Float t;
            if (!intersector(ray, surface, t)) {
                return false;
            }
            if (t > 1e+5) {
                /**
                 * Large distances create numerical instability.
                 * We work around this problem by intersecting another time from a closer point.
                 * @todo this needs more thorough investigation
                 */
                ray.origin = ray(t - 1e+2);
                if (!intersector(ray, surface, t)) {
                    return false;
                }
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
