#pragma once

#include <lore/lore.h>
#include <lore/lens/Lens.h>

namespace lore {

template<typename Float>
Float petzvalCurvature(const Lens<Float> &lens, Float wavelength) {
    Float sum = 0;
    Float n1 = 1;

    for (const auto &surface : lens.surfaces) {
        Float n2 = surface.ior(wavelength);
        sum += surface.curvature() * (n1 - n2) / (n1 * n2);
        n1 = n2;
    }

    return sum;
}

template<typename Float>
Float petzvalRadius(const Lens<Float> &lens, Float wavelength) {
    return 1 / petzvalCurvature(lens, wavelength);
}

}
