#pragma once

#include <lore/lore.h>
#include <lore/lens/LensSchema.h>
#include <lore/lens/GlassCatalog.h>

#include <istream>
#include <vector>

namespace lore {
namespace io {

class LensReader {
public:
    const GlassCatalog &glassCatalog;

    LensReader() : glassCatalog(GlassCatalog::shared) {}
    LensReader(const GlassCatalog &glassCatalog) : glassCatalog(glassCatalog) {}

    std::vector<LensSchema<float>> read(std::istream &is) const;
};

}
}
