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
    Float fieldAngle;
    Float entranceBeamRadius;
    int stopIndex;

    std::vector<SurfaceSchema<Float>> surfaces;
    std::vector<WeightedWavelength<Float>> wavelengths;

    template<typename LFloat>
    Lens<LFloat> lens() const {
        Lens<LFloat> result;
        for (const auto &surface : surfaces) {
            result.surfaces.emplace_back(
                surface.radius,
                surface.thickness,
                surface.aperture,
                surface.checkAperture,
                surface.glass.template cast<LFloat>()
            );
        }
        return result;
    }

    Float primaryWavelength() const {
        return wavelengths.front().wavelength;
    }
};

}
