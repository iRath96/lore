#pragma once

#include <lore/lore.h>
#include <lore/rt/ABCD.h>

namespace lore {

template<typename Float>
struct ParaxialAnalysis {
    Float efl;
    Float focalShift;

    ParaxialAnalysis(const Lens<Float> &lens, Float wavelength)
    : efl(0), focalShift(0) {
        const Matrix2x2<Float> rt = abcd::full(lens, wavelength);
        efl = Float(1) / -rt(1, 0);
        focalShift = rt(0, 0) / -rt(1, 0);
        // | h/h h/s |
        // | s/h s/s |
    }
};

}
