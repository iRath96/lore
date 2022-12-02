#include <lore/lore.h>
#include <lore/math.h>
#include <lore/rt/Ray.h>
#include <lore/lens/Lens.h>

namespace lore {
namespace rt {

template<typename Float>
struct SequentialTrace {
    Float wavelength;

    SequentialTrace(MTL_THREAD const Float &wavelength)
        : wavelength(wavelength) {}

    template<typename Intersector>
    bool operator()(MTL_THREAD Ray<Float> &ray, MTL_THREAD const Lens<Float> &lens, MTL_THREAD const Intersector &intersector) const {
        Float n1(1);

        bool firstSurface = true;
        for (const MTL_DEVICE lore::Surface<Float> &surface : lens.surfaces) {
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

template<typename Float>
struct InverseSequentialTrace {
    Float wavelength;

    InverseSequentialTrace(MTL_THREAD const Float &wavelength)
        : wavelength(wavelength) {}

    template<typename Intersector>
    bool operator()(MTL_THREAD Ray<Float> &ray, MTL_THREAD const Lens<Float> &lens, MTL_THREAD const Intersector &intersector) const {
        int surfaceIndex = lens.surfaces.size() - 1;
        Float n2 = lens.surfaces[surfaceIndex].ior(wavelength);
        for (; surfaceIndex > 0; surfaceIndex--) {
            MTL_DEVICE auto &surface = lens.surfaces[surfaceIndex];
            const Float n1 = lens.surfaces[surfaceIndex - 1].ior(wavelength);
            ray.origin.z() += surface.thickness;
            
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
            if (!refract(ray.direction, normal, ray.direction, n2 / n1)) {
                return false;
            }

            n2 = n1;
        }

        return true;
    }
};

}
}
