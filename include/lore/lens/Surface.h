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

    Float ior(Float wavelength) const {
        return glass.ior(wavelength);
    }

    Float curvature() const {
        return radius == 0 ? 0 : 1 / radius;
    }
};

}