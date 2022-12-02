#pragma once

#include <lore/lore.h>
#include <lore/lens/Glass.h>

#include <string>
#include <unordered_map>
#include <fstream>

namespace lore {

struct GlassCatalog {
    static GlassCatalog shared;

    struct Entry {
        std::string name;
        float ior;
        float Vno; // dFC
        float density; // gm/cc
        float dndT; // *1e+6
        int TCE; // *1e+7
        float Xmittance; // 25mm
        float cost;
        int hardness;
        int chemical;

        Glass<float> glass;
    };

    std::unordered_map<std::string, Entry> data = {};

    int read(const std::string &path);
    int read(std::ifstream &is);

    Glass<float> glass(const std::string &name) const {
        auto result = data.find(name);
        if (result == data.end()) {
            std::cerr << "unknown glass '" << name << "'" << std::endl;
            return Glass<float>::air();
        }

        return result->second.glass;
    }
};

}
