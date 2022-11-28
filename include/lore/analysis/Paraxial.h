#pragma once

#include <lore/lore.h>
#include <lore/rt/ABCD.h>

namespace lore {

template<typename Float>
struct ParaxialAnalysis {
    const Float efl;
    const Float focalShift;

    ParaxialAnalysis(const Lens<Float> &lens, Float wavelength) {
        Matrix2x2<Float> rt = abcd::full(lens, wavelength);
        efl = 1 / -rt(0, 1);
        focalShift = rt(0, 0) / -rt(0, 1);
    }
};

}
