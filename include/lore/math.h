#pragma once

#include <lore/lore.h>
#include <cmath>
#include <iostream>

namespace lore {

template<typename Float, typename = void>
struct math {
};

template<typename Float>
struct math<Float, std::enable_if_t<std::is_floating_point<Float>::value>> {
    typedef Float detached;
    static Float sqrt(Float v) { return std::sqrt(v); }
    static Float copysign(Float mag, Float sgn) { return std::copysign(mag, sgn); }
    static Float detach(Float v) { return v; }
};

template<typename Float>
Float sqr(Float v) { return v * v; }

template<typename Float>
Float sqrt(Float v) { return math<Float>::sqrt(v); }

template<typename Float>
Float copysign(Float mag, Float sgn) { return math<Float>::copysign(mag, sgn); }

template<typename Float>
typename math<Float>::detached detach(Float v) { return math<Float>::detach(v); }

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
        auto it = l.begin();
        for (int i = 0; i < N; i++) {
            el[i] = *it;
            it++;
        }
    }

    Float &operator()(int i) {
        return el[i];
    }

    const Float &operator()(int i) const {
        return el[i];
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

    Vector normalized() const {
        return *this / length();
    }

    Vector &operator*=(const Float &other) {
        for (int i = 0; i < N; i++) {
            el[i] *= other;
        }
        return *this;
    }

    Vector &operator/=(const Float &other) {
        return (*this *= Float(1) / other);
    }

    Vector &operator+=(const Vector &other) {
        for (int i = 0; i < N; i++) {
            (*this)(i) += other(i);
        }
        return *this;
    }

    Vector &operator-=(const Vector &other) {
        for (int i = 0; i < N; i++) {
            (*this)(i) -= other(i);
        }
        return *this;
    }

    Vector operator*(const Float &other) const {
        Vector copy = *this;
        copy *= other;
        return copy;
    }

    Vector operator/(const Float &other) const {
        Vector copy = *this;
        copy /= other;
        return copy;
    }

    Vector operator+(const Vector &other) const {
        Vector copy = *this;
        copy += other;
        return copy;
    }

    Vector operator-(const Vector &other) const {
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

    friend Vector operator*(const Float &lhs, Vector rhs) {
        return rhs *= lhs;
    }

    bool operator==(const Vector &other) const {
        for (int i = 0; i < N; i++) {
            if ((*this)(i) != other(i)) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Vector &other) const {
        return !(*this == other);
    }
};

template<typename Float, int N>
std::ostream &operator <<(std::ostream &os, Vector<Float, N> const &value) {
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

template<typename Float, int Rows, int Columns>
struct Matrix {
    Float el[Rows][Columns];

    Float &operator()(int row, int column) {
        return el[row][column];
    }

    const Float &operator()(int row, int column) const {
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
    Matrix<Float, Rows, Columns2> operator*(const Matrix<Float, Rows2, Columns2> &other) const {
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

    bool operator==(const Matrix &other) const {
        for (int row = 0; row < Rows; row++) {
            for (int column = 0; column < Columns; column++) {
                if ((*this)(row, column) != other(row, column)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix &other) const {
        return !(*this == other);
    }
};

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

template<typename Float>
struct Matrix2x2 : public Matrix<Float, 2, 2> {
    Matrix2x2() : Matrix<Float, 2, 2>() {}
    Matrix2x2(const Matrix<Float, 2, 2> &other) : Matrix<Float, 2, 2>(other) {}

    Matrix2x2(Float a, Float b, Float c, Float d) {
        (*this)(0, 0) = a;
        (*this)(0, 1) = b;
        (*this)(1, 0) = c;
        (*this)(1, 1) = d;
    }
};

template<typename Float>
bool refract(const Vector3<Float> &incident, const Vector3<Float> &normal, Vector3<Float> &result, Float eta) {
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
Vector<Float, N> faceforward(const Vector<Float, N> &n, const Vector<Float, N> &d) {
    return n.dot(d) > 0 ? n : -n;
}

}
