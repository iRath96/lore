#pragma once

#include <lore/lore.h>
#include <lore/lens/Glass.h>

#include <string>
#include <unordered_map>

namespace lore {

struct GlassCatalog {
    std::unordered_map<std::string, Glass<>> data = {{

    }};
};

}
