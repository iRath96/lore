#pragma once

#include <lore/lore.h>
#include <lore/math.h>

namespace lore {
namespace optim {

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

    FADFloat operator-() const {
        return FADFloat(-V, -dVd);
    }

    FADFloat operator+=(const FADFloat &other) {
        return (*this = *this + other);
    }

    FADFloat operator-=(const FADFloat &other) {
        return (*this = *this - other);
    }

    FADFloat operator*=(const FADFloat &other) {
        return (*this = *this * other);
    }

    FADFloat operator/=(const FADFloat &other) {
        return (*this = *this / other);
    }

    bool operator>(const FADFloat &other) const { return V > other.V; }
    bool operator<(const FADFloat &other) const { return V < other.V; }
    bool operator==(const FADFloat &other) const { return V == other.V; }
    bool operator!=(const FADFloat &other) const { return V != other.V; }
};

template<typename Float, int N>
std::ostream &operator<<(std::ostream &os, optim::FADFloat<Float, N> const &value) {
    os << "FAD<" << N << ">{ " << std::to_string(value.V) << ", { ";
    for (int i = 0; i < N; i++) {
        if (i) os << ", ";
        os << value.dVd(i);
    }
    os << " } }";
    return os;
}

}

template<typename Float, int N>
struct math<optim::FADFloat<Float, N>> {
    using FAD = optim::FADFloat<Float, N>;
    using Detached = Float;

    static FAD sin(FAD v) {
        return {math<Float>::sin(v.V), v.dVd * math<Float>::cos(v.V)};
    }

    static FAD cos(FAD v) {
        return {math<Float>::cos(v.V), -v.dVd * math<Float>::sin(v.V)};
    }

    static FAD sqrt(FAD v) {
        const Float root = math<Float>::sqrt(v.V);
        return {root, v.dVd / (2 * root)};
    }

    static FAD copysign(FAD mag, FAD sgn) {
        return std::copysign(mag.V, sgn.V);
    }

    static Float detach(FAD v) {
        return v.V;
    }
};

}
