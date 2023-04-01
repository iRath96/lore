#pragma once

#include <lore/lore.h>

#ifndef __METAL__
#include <cmath>
#include <iostream>
#endif

namespace lore {

template<typename Float, typename = void>
struct math {
};

template<typename Float>
struct math<Float, std::enable_if_t<std::is_floating_point<Float>::value>> {
    using Detached = Float;

    static Float sin(Float v) { return std::sin(v); }
    static Float cos(Float v) { return std::cos(v); }
    static Float sqrt(Float v) { return std::sqrt(v); }
    static Float copysign(Float mag, Float sgn) { return std::copysign(mag, sgn); }
    static Float detach(Float v) { return v; }
};

template<typename Float> Float sqr(Float v) { return v * v; }
template<typename Float> Float sqrt(Float v) { return math<Float>::sqrt(v); }
template<typename Float> Float copysign(Float mag, Float sgn) { return math<Float>::copysign(mag, sgn); }
template<typename Float> Float sin(Float v) { return math<Float>::sin(v); }
template<typename Float> Float cos(Float v) { return math<Float>::cos(v); }

template<typename Float>
typename math<Float>::Detached detach(Float v) { return math<Float>::detach(v); }

template<typename Float, int N>
struct Vector {
    Float el[N];

    Vector() {
        for (int i = 0; i < N; i++) {
            el[i] = 0;
        }
    }

    Vector(Float v) {
        for (int i = 0; i < N; i++) {
            el[i] = v;
        }
    }

    Vector(std::initializer_list<Float> l) {
        //static_assert(l.size() == N, "incorrect number of components to Vector constructor");

        auto it = l.begin();
        for (int i = 0; i < N; i++) {
            el[i] = *it;
            it++;
        }
    }

    MTL_THREAD Float &operator()(int i) {
        return el[i];
    }

    MTL_THREAD const Float &operator()(int i) const {
        return el[i];
    }

#define MAKE_ACCESSOR(name, index) \
MTL_THREAD Float &name() { \
    static_assert(index < N, "out of bounds"); \
    return this->el[index]; \
} \
MTL_THREAD const Float &name() const { \
    static_assert(index < N, "out of bounds"); \
    return this->el[index]; \
}

    MAKE_ACCESSOR(x, 0)
    MAKE_ACCESSOR(y, 1)
    MAKE_ACCESSOR(z, 2)
    MAKE_ACCESSOR(w, 3)

#undef MAKE_ACCESSOR

    Float lengthSquared() const {
        return dot(*this);
    }

    Float length() const {
        return sqrt(lengthSquared());
    }

    Float dot(MTL_THREAD const Vector<Float, N> &other) const {
        Float sum(0);
        for (int i = 0; i < N; i++) {
            sum += el[i] * other.el[i];
        }
        return sum;
    }

    Vector normalized() const {
        return *this / length();
    }

    Float sum() const {
        Float result(0);
        for (int i = 0; i < N; i++) {
            result += el[i];
        }
        return result;
    }

    MTL_THREAD Vector &operator*=(MTL_THREAD const Float &other) {
        for (int i = 0; i < N; i++) {
            el[i] *= other;
        }
        return *this;
    }

    MTL_THREAD Vector &operator*=(MTL_THREAD const Vector &other) {
        for (int i = 0; i < N; i++) {
            el[i] *= other(i);
        }
        return *this;
    }

    MTL_THREAD Vector &operator/=(MTL_THREAD const Float &other) {
        return (*this *= Float(1) / other);
    }

    MTL_THREAD Vector &operator+=(MTL_THREAD const Vector &other) {
        for (int i = 0; i < N; i++) {
            (*this)(i) += other(i);
        }
        return *this;
    }

    MTL_THREAD Vector &operator-=(MTL_THREAD const Vector &other) {
        for (int i = 0; i < N; i++) {
            (*this)(i) -= other(i);
        }
        return *this;
    }

    MTL_THREAD Vector operator*(MTL_THREAD const Float &other) const {
        Vector copy = *this;
        copy *= other;
        return copy;
    }

    MTL_THREAD Vector operator*(MTL_THREAD const Vector &other) const {
        Vector copy = *this;
        copy *= other;
        return copy;
    }

    MTL_THREAD Vector operator/(MTL_THREAD const Float &other) const {
        Vector copy = *this;
        copy /= other;
        return copy;
    }

    Vector operator+(MTL_THREAD const Vector &other) const {
        Vector copy = *this;
        copy += other;
        return copy;
    }

    Vector operator-(MTL_THREAD const Vector &other) const {
        Vector copy = *this;
        copy -= other;
        return copy;
    }

    Vector operator-() const {
        Vector copy;
        for (int i = 0; i < N; i++) {
            copy(i) = -(*this)(i);
        }
        return copy;
    }

    friend Vector operator*(MTL_THREAD const Float &lhs, Vector rhs) {
        return rhs *= lhs;
    }

    bool operator==(MTL_THREAD const Vector &other) const {
        for (int i = 0; i < N; i++) {
            if ((*this)(i) != other(i)) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(MTL_THREAD const Vector &other) const {
        return !(*this == other);
    }
};

template<typename Float, int N>
struct math<Vector<Float, N>> {
    using Detached = Vector<typename math<Float>::Detached, N>;
    using Base = Vector<Float, N>;

    static Base sqrt(MTL_THREAD const Base &v) {
        Base result;
        for (int i = 0; i < N; i++) result(i) = sqrt(v(i));
        return result;
    }
    static Detached detach(MTL_THREAD const Base &v) {
        Detached result;
        for (int i = 0; i < N; i++) result(i) = lore::detach(v(i));
        return result;
    }
};

#ifndef __METAL__
template<typename Float, int N>
std::ostream &operator<<(std::ostream &os, Vector<Float, N> const &value) {
    os << "Vector<" << N << ">{ ";
    for (int i = 0; i < N; i++) {
        if (i > 0) {
            os << ", ";
        }
        os << value(i);
    }
    os << " }";
    return os;
}
#endif

template<typename Float, int Rows, int Columns>
struct Matrix {
    Float el[Rows][Columns];

    Matrix() {}
    Matrix(std::initializer_list<Float> l) {
        //static_assert(l.size() == (Rows * Columns), "incorrect number of components to Matrix constructor");

        auto it = l.begin();
        for (int row = 0; row < Rows; row++) {
            for (int column = 0; column < Columns; column++) {
                el[row][column] = *it;
                it++;
            }
        }
    }

    MTL_THREAD Float &operator()(int row, int column) {
        return el[row][column];
    }

    MTL_THREAD const Float &operator()(int row, int column) const {
        return el[row][column];
    }

    static Matrix Identity() {
        Matrix result;
        for (int row = 0; row < Rows; row++) {
            for (int column = 0; column < Columns; column++) {
                result(row, column) = (row == column) ? Float(1) : Float(0);
            }
        }
        return result;
    }

    template<int Rows2, int Columns2>
    Matrix<Float, Rows, Columns2> operator*(MTL_THREAD const Matrix<Float, Rows2, Columns2> &other) const {
        static_assert(Columns == Rows2, "columns of first matrix must match rows of second matrix");

        Matrix<Float, Rows, Columns2> result;
        for (int row = 0; row < Rows; row++) {
            for (int column = 0; column < Columns2; column++) {
                result(row, column) = Float(0);
                for (int t = 0; t < Columns; t++) {
                    result(row, column) += (*this)(row, t) * other(t, column);
                }
            }
        }
        return result;
    }

    bool operator==(MTL_THREAD const Matrix &other) const {
        for (int row = 0; row < Rows; row++) {
            for (int column = 0; column < Columns; column++) {
                if ((*this)(row, column) != other(row, column)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(MTL_THREAD const Matrix &other) const {
        return !(*this == other);
    }
};

#ifndef __METAL__
template<typename Float, int Rows, int Columns>
std::ostream &operator <<(std::ostream &os, Matrix<Float, Rows, Columns> const &value) {
    os << "Matrix<" << Rows << "x" << Columns << ">{" << std::endl;
    for (int row = 0; row < Rows; row++) {
        os << "  ";
        for (int column = 0; column < Columns; column++) {
            os << value(row, column) << ", ";
        }
        os << std::endl;
    }
    os << "}";
    return os;
}
#endif

template<typename Float> using Vector2 = Vector<Float, 2>;
template<typename Float> using Vector3 = Vector<Float, 3>;
template<typename Float> using Vector4 = Vector<Float, 4>;
template<typename Float> using Matrix2x2 = Matrix<Float, 2, 2>;

template<typename Float>
bool refract(MTL_THREAD const Vector3<Float> &incident, MTL_THREAD const Vector3<Float> &normal, MTL_THREAD Vector3<Float> &result, Float eta) {
    const Float NdotI = normal.dot(incident);
    const Float k = Float(1) - sqr(eta) * (Float(1) - sqr(NdotI));
    if (k < 0) {
        // total internal reflection
        return false;
    }

    result = eta * incident - (eta * NdotI + sqrt(k)) * normal;
    return true;
}

template<typename Float, int N>
Vector<Float, N> faceforward(MTL_THREAD const Vector<Float, N> &n, MTL_THREAD const Vector<Float, N> &d) {
    return n.dot(d) > 0 ? n : -n;
}

#ifdef __METAL__
#define M_PI M_PI_F
#endif

template<typename Float>
Vector2<Float> polar(Float phi, Float r) {
    phi *= 2 * M_PI;
    return { r * std::cos(phi), r * std::sin(phi) };
}

}
