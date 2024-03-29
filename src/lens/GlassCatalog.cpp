#include <lore/io/LensReader.h>
#include <lore/lens/GlassCatalog.h>
#include <lore/logging.h>

#include <iostream>
#include <sstream>
#include <exception>

namespace lore {

GlassCatalog GlassCatalog::shared;

enum IORType {
    IOR_TYPE_LAURENT = 1,
    IOR_TYPE_SELLMEIER = 2,
};

void expectLinebreak(std::ifstream &is) {
    while (true) {
        if (!isspace(is.peek())) {
            std::string remainder;
            std::getline(is, remainder);
            remainder.pop_back();
            log::warning() << "expected linebreak, but found '" << remainder << "'" << std::flush;
            break;
        }

        int next = is.get();
        if (next == EOF || next == '\n') {
            break;
        }
    }
}

std::vector<float> readVector(std::ifstream &is, int multiplier = 1) {
    std::vector<float> result;
    int numElements;

    is >> numElements;
    result.resize(numElements * multiplier);

    for (auto &e : result) {
        is >> e;
    }

    return result;
}

void readUnknown3(std::ifstream &is) {
    int unk;
    is >> unk;

    readVector(is);
}

void readUnknown4(std::ifstream &is) {
    int unk;
    is >> unk;

    readVector(is, 2);
}

void readUnknown5(std::ifstream &is) {
    const std::ios::pos_type pos = is.tellg();

    int unk;
    is >> unk;
    if (is.fail()) {
        is.clear();
        is.seekg(pos);
        return;
    }

    readVector(is);

    is >> unk >> unk >> unk;
}

Glass<float> readGlass(std::ifstream &is) {
    int type;
    is >> type;

    const auto coeff = readVector(is);
    if (type == IOR_TYPE_LAURENT) {
        if (coeff.size() != 6) {
            log::error() << "unsupported number of Laurent coefficients" << std::flush;
            return Glass<float>::air();
        }

        return Glass<float>(LaurentIOR<2, 4, float>({
            coeff[0], coeff[1],
            coeff[2], coeff[3], coeff[4], coeff[5]
        }));
    }

    if (type == IOR_TYPE_SELLMEIER) {
        if (coeff.size() != 6) {
            log::error() << "unsupported number of Sellmeier coefficients" << std::flush;
            return Glass<float>::air();
        }

        return Glass<float>(SellmeierIOR<3, float>({
            coeff[0], coeff[1], coeff[2]
        }, {
            coeff[3], coeff[4], coeff[5]
        }));
    }

    log::error() << "unsupported glass type '" << type << "'" << std::flush;
    return Glass<float>::air();
}

int GlassCatalog::read(const std::string &path) {
    std::ifstream file { path };
    return read(file);
}

// trim functions taken from https://stackoverflow.com/a/217605

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

int GlassCatalog::read(std::ifstream &is) {
    std::string version, catalogName;
    int numElements;
    is >> version >> numElements;
    std::getline(is, catalogName);
    trim(catalogName);

    auto &list = byCatalog[catalogName];
    for (int i = 0; i < numElements; i++) {
        Entry entry;
        is
            >> entry.name
            >> entry.ior
            >> entry.Vno
            >> entry.density
            >> entry.dndT
            >> entry.TCE
            >> entry.Xmittance
            >> entry.cost
            >> entry.hardness
            >> entry.chemical;

        std::string unk1, unk2;
        is >> unk1 >> unk2;

        entry.glass = readGlass(is);

        readUnknown3(is);
        readUnknown4(is);
        readUnknown5(is);

        if (is.fail()) {
            log::error() << "stream failure for '" << entry.name << "'" << std::flush;
            is.clear();
        }

        expectLinebreak(is);

        list.push_back(entry.name);
        data[entry.name] = entry;
    }

    log::info() << "read " << numElements << " glass definitions from '" << catalogName << "'" << std::flush;
    return numElements;
}

}
