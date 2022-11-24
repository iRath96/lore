#pragma once

#include <lore/lore.h>

namespace lore {

template<typename Float = float>
struct LensElement {
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

    MaterialIndex material;
};

}
