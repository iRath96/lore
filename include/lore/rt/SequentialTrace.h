#include <lore/lore.h>
#include <lore/math.h>
#include <lore/rt/Ray.h>
#include <lore/lens/Lens.h>

namespace lore {
namespace rt {

template<typename Float>
struct TraceUtils {
    template<typename Intersector>
    static bool propagate(
        MTL_THREAD Ray<Float> &ray,
        MTL_DEVICE const Surface<Float> &surface,
        MTL_THREAD const Intersector &intersector
    ) {
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

        return true;
    }

    static Vector3<Float> normal(
        MTL_THREAD const Ray<Float> &ray,
        MTL_DEVICE const Surface<Float> &surface
    ) {
        if (surface.isFlat()) {
            return Vector3<Float>{0, 0, -copysign(Float(1), ray.direction.z())};
        }
        return -faceforward(
            (ray.origin - Vector3<Float>{0, 0, surface.radius}).normalized(),
            ray.direction
        );
    }
};

template<typename Float, typename Intersector>
struct SequentialTrace {
    SequentialTrace(
        MTL_THREAD const Lens<Float> &lens,
        MTL_THREAD const Intersector &intersector,
        MTL_THREAD const Float &wavelength
    ) : firstSurface(1),
        lastSurface(int(lens.surfaces.size()) - 1),
        lens(lens),
        intersector(intersector),
        wavelength(wavelength) {}

    SequentialTrace(
        MTL_THREAD const Lens<Float> &lens,
        MTL_THREAD const Intersector &intersector,
        MTL_THREAD const Float &wavelength,
        int firstSurface,
        int lastSurface
    ) : firstSurface(firstSurface),
        lastSurface(lastSurface),
        lens(lens),
        intersector(intersector),
        wavelength(wavelength) {}

    bool operator()(MTL_THREAD Ray<Float> &ray) const {
        Float n1 = lens.surfaces.front().ior(wavelength);

        for (int i = firstSurface; i <= lastSurface; i++) {
            const MTL_DEVICE lore::Surface<Float> &surface = lens.surfaces[i];
            if (!TraceUtils<Float>::propagate(ray, surface, intersector)) {
                return false;
            }

            const Vector3<Float> normal = TraceUtils<Float>::normal(ray, surface);
            const Float n2 = surface.ior(wavelength);
            if (!refract(ray.direction, normal, ray.direction, n1 / n2)) {
                return false;
            }

            ray.origin.z() -= surface.thickness;
            n1 = n2;
        }

        return true;
    }

private:
    int firstSurface;
    int lastSurface;

    Float wavelength;
    MTL_THREAD const Lens<Float> &lens;
    MTL_THREAD const Intersector &intersector;
};

template<typename Float>
struct InverseSequentialTrace {
    Float wavelength;

    InverseSequentialTrace(MTL_THREAD const Float &wavelength)
        : wavelength(wavelength) {}

    template<typename Intersector>
    bool operator()(MTL_THREAD Ray<Float> &ray, MTL_THREAD const Lens<Float> &lens, MTL_THREAD
                    const Intersector &intersector) const {
        int surfaceIndex = lens.surfaces.size() - 1;
        Float n2 = lens.surfaces[surfaceIndex].ior(wavelength);
        for (; surfaceIndex > 0; surfaceIndex--) {
            MTL_DEVICE auto &surface = lens.surfaces[surfaceIndex];
            ray.origin.z() += surface.thickness;

            if (!TraceUtils<Float>::propagate(ray, surface, intersector)) {
                return false;
            }

            const Vector3<Float> normal = TraceUtils<Float>::normal(ray, surface);
            const Float n1 = lens.surfaces[surfaceIndex - 1].ior(wavelength);
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
