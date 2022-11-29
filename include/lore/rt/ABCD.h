#pragma once

#include <lore/lore.h>
#include <lore/math.h>
#include <lore/lens/Lens.h>

namespace lore {
namespace abcd {

template<typename Float>
Matrix2x2<Float> refraction(Float n1, Float n2, Float curvature) {
    return Matrix2x2<Float>(
        1, 0,
        curvature * (n1 - n2) / n2, n1 / n2
    );
}

template<typename Float>
Matrix2x2<Float> propagation(Float t) {
    return Matrix2x2<Float>(
        1, t,
        0, 1
    );
}

template<typename Float>
Matrix2x2<Float> full(const Lens<Float> &lens, Float wavelength) {
    Matrix2x2<Float> result = Matrix2x2<Float>::Identity();
    Float n1 = 1;
    for (const auto &surface : lens.surfaces) {
        const Float n2 = surface.ior(wavelength);
        result = refraction(n1, n2, surface.curvature()) * result;
        result = propagation(surface.thickness) * result;
        n1 = n2;
    }
    return result;
}

}
}
