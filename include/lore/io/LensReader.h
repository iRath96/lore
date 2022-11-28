#pragma once

#include <lore/lore.h>
#include <lore/lens/LensSchema.h>

#include <istream>
#include <vector>

namespace lore::io {

class LensReader {
public:
    std::vector<LensSchema<float>> read(std::istream &is) const;
};

}
