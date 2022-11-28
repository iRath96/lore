#pragma once

#include <lore/lore.h>
#include <lore/lens/Lens.h>
#include <lore/lens/SurfaceSchema.h>

#include <string>
#include <vector>

namespace lore {

template<typename Float>
struct WeightedWavelength {
    Float wavelength;
    Float weight;

    WeightedWavelength()
    : wavelength(0), weight(0) {}

    WeightedWavelength(Float wavelength, Float weight)
    : wavelength(wavelength), weight(weight) {}
};

template<typename Float>
struct LensSchema {
    std::string name;
    std::string description;

    std::vector<SurfaceSchema<Float>> surfaces;
    std::vector<WeightedWavelength<Float>> wavelengths;

    Lens<Float> lens() const {
        Lens<Float> result;
    }
};

}
