#pragma once

#include <lore/lore.h>

namespace lore {

template<typename Float>
Float sqr(Float v) {
    return v * v;
}

template<typename Float, int N, int M>
struct Matrix {
    Float el[N][M];
};

template<typename Float>
using Matrix2x2 = Matrix<Float, 2, 2>;

}
