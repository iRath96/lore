#pragma once

#include <lore/lore.h>
#include <lore/lens/Glass.h>

namespace lore {

template<typename Float = float>
struct Surface {
    /**
     * Radius of curvature of this element or zero for flat surfaces.
     */
    Float radius;

    /**
     * Distance on the optical axis to the next element.
     */
    Float thickness;

    /**
     * Aperture radius.
     * @note A value of zero indicates that the aperture should be solved for.
     */
    Float aperture;

    /**
     * Whether the aperture radius is checked for ray tracing operations.
     */
    bool checkAperture;

    /**
     * The material on the right of the surface.
     */
    Glass<Float> glass;

    Surface()
    : radius(0), thickness(0), aperture(0), checkAperture(false), glass(Glass<Float>::air()){
    }

    Surface(Float radius, Float thickness, Float aperture, bool checkAperture, MTL_THREAD const Glass<Float> &glass)
    : radius(radius), thickness(thickness), aperture(aperture), checkAperture(checkAperture), glass(glass) {}

    Float ior(Float wavelength) const MTL_DEVICE {
        return glass.ior(wavelength);
    }

    bool isFlat() const MTL_DEVICE {
        return radius == 0;
    }

    bool needsApertureSolve() const {
        return aperture == 0;
    }

    Float curvature() const {
        return isFlat() ? 0 : 1 / radius;
    }
};

}
