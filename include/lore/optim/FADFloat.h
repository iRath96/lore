#pragma once

#include <lore/lore.h>
#include <lore/math.h>

namespace lore::optim {

template<typename Float, int N>
struct FADFloat {
    Float V;
    Vector<Float, N> dVd;

    FADFloat()
    : V(), dVd() {}

    FADFloat(Float V)
    : V(V), dVd() {}

    FADFloat(Float V, const Vector<Float, N> &dVd)
    : V(V), dVd(dVd) {}

    FADFloat operator+(const FADFloat &other) const {
        return FADFloat(V + other.V, dVd + other.dVd);
    }

    FADFloat operator-(const FADFloat &other) const {
        return FADFloat(V - other.V, dVd - other.dVd);
    }

    FADFloat operator*(const FADFloat &other) const {
        return FADFloat(V * other.V, dVd * other.V + other.dVd * V);
    }

    FADFloat operator/(const FADFloat &other) const {
        return FADFloat(V / other.V, (dVd * other.V - other.dVd * V) / sqr(other.V));
    }

    FADFloat operator+=(const FADFloat &other) {
        return (*this = *this + other);
    }
};

}

namespace lore {

template<typename Float, int N>
struct math<optim::FADFloat<Float, N>> {
    using FAD = optim::FADFloat<Float, N>;
    static FAD sqrt(FAD v) {
        const Float root = math<Float>::sqrt(v.V);
        return optim::FADFloat<Float, N> { root, v.dVd / (2 * root) };
    }
};

}
