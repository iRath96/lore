#pragma once

#include <lore/lore.h>
#include <cmath>

namespace lore {

template<typename Float>
Float sqr(Float v) {
    return v * v;
}

template<typename Float>
Float sqrt(Float v) {
    return std::sqrt(v);
}

template<typename Float, int N>
struct Vector {
    Float el[N];

    Vector() {
        for (int i = 0; i < N; i++) {
            el[i] = 0;
        }
    }

    Float lengthSquared() const {
        return dot(*this);
    }

    Float length() const {
        return sqrt(lengthSquared());
    }

    Float dot(const Vector<Float, N> &other) const {
        Float sum(0);
        for (int i = 0; i < N; i++) {
            sum += el[i] * other.el[i];
        }
        return sum;
    }

    Vector<Float, N> normalized() const {
        return *this / length();
    }

    Vector<Float, N> &operator*=(const Float &other) {
        for (int i = 0; i < N; i++) {
            el[i] *= other;
        }
        return *this;
    }

    Vector<Float, N> &operator/=(const Float &other) {
        return (*this *= Float(1) / other);
    }

    Vector<Float, N> operator*(const Float &other) const {
        Vector<Float, N> copy = *this;
        copy *= other;
        return copy;
    }

    Vector<Float, N> operator/(const Float &other) const {
        Vector<Float, N> copy = *this;
        copy /= other;
        return copy;
    }
};

template<typename Float, int N, int M>
struct Matrix {
    Float el[N][M];
};

template<typename Float>
using Matrix2x2 = Matrix<Float, 2, 2>;

template<typename Float>
struct Vector3 : public Vector<Float, 3> {
    Vector3() : Vector<Float, 3>() {}
    Vector3(const Vector<Float, 3> &other) : Vector<Float, 3>(other) {}

    Vector3(Float x, Float y, Float z) {
        this->el[0] = x;
        this->el[1] = y;
        this->el[2] = z;
    }

    Float &x() { return this->el[0]; }
    Float &y() { return this->el[1]; }
    Float &z() { return this->el[2]; }

    const Float &x() const { return this->el[0]; }
    const Float &y() const { return this->el[1]; }
    const Float &z() const { return this->el[2]; }
};

}
