#pragma once

#include <lore/lore.h>
#include <lore/lens/Surface.h>

#include <vector>

namespace lore {

template<typename Float = float>
struct Lens {
    std::vector<Surface<Float>> surfaces;
};

}
