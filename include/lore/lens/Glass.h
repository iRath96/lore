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
     * Computes the Sellmeier equation for a given wavelength in micrometers.
     */
    Float operator()(Float wavelength) const {
        const Float wSqr = sqr(wavelength);
        Float nSqr = 1;
        for (int i = 0; i < N; i++) {
            nSqr += B[i] * wSqr / (wSqr - C[i]);
        }
        return sqrt(nSqr);
    }

    static SellmeierIOR air() {
        SellmeierIOR result;
        for (int i = 0; i < N; i++) {
            result.B[i] = 0;
            result.C[i] = 0;
        }
        return result;
    }

    static SellmeierIOR constant(Float ior) {
        SellmeierIOR result = air();
        result.B[0] = sqr(ior) - 1;
        return result;
    }
};

template<typename Float = float>
struct Glass {
    typedef SellmeierIOR<3, Float> IOR;
    
    IOR ior;

    Glass() : ior(IOR::air()) {}
    explicit Glass(const IOR &ior) : ior(ior) {}

    static Glass air() {
        return Glass(IOR::air());
    }

    static Glass constant(Float ior) {
        return Glass(IOR::constant(ior));
    }
};

}
