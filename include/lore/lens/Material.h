#pragma once

#include <lore/lore.h>
#include <lore/math.h>

#include <string>
#include <vector>

namespace lore {

template<int N, typename Float = float>
struct SellmeierEquation {
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
};

template<typename Float = float>
struct Material {
    std::string name;
    SellmeierEquation<3, Float> sellmeier;
    
    Float ior(Float wavelength) {
        return sellmeier(wavelength);
    }
};

typedef int MaterialIndex;

template<typename Float = float>
struct MaterialCatalog {
    std::vector<Material<Float>> materials;
};

}
