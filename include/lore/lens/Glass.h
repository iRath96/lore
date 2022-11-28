#pragma once

#include <lore/lore.h>
#include <lore/math.h>

#include <string>

namespace lore {

template<int N, typename Float = float>
struct SellmeierIOR {
    Float B[N];
    Float C[N];

    /**
     * Computes the sellmeier equation for a given wavelength in micrometers.
     */
    Float operator()(Float wavelength) {
        const Float wSqr = sqr(wavelength);
        Float nSqr = 1;
        for (int i = 0; i < N; i++) {
            nSqr += B[i] * wSqr / (wSqr - C[i]);
        }
        return sqrt(nSqr);
    }

    static SellmeierIOR<N, Float> air() {
        SellmeierIOR<N, Float> result;
        for (int i = 0; i < N; i++) {
            result.B[i] = 0;
            result.C[i] = 0;
        }
        return result;
    }
};

template<typename Float = float>
struct Glass {
    typedef SellmeierIOR<3, Float> IOR;
    
    IOR ior;

    static Glass<Float> air() {
        Glass<Float> glass;
        glass.ior = IOR::air();
        return glass;
    }
};

}
