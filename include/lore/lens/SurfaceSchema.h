#pragma once

#include <lore/lore.h>
#include <lore/lens/Surface.h>

namespace lore {

template<typename Float = float>
struct SurfaceSchema : public Surface<Float> {
};

}
