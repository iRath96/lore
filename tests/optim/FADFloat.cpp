#include <catch2/catch_test_macros.hpp>

#include <lore/lore.h>
#include <lore/optim/FADFloat.h>

using namespace lore;
using namespace lore::optim;

TEST_CASE( "Autodiff constants", "[autodiff]" ) {
    using Float = FADFloat<float, 2>;

    Float v = Float(2);
    REQUIRE( v.V == 2 );
    REQUIRE( v.dVd(0) == 0 );
    REQUIRE( v.dVd(1) == 0 );
}

TEST_CASE( "Autodiff addition", "[autodiff]" ) {
    using Float = FADFloat<float, 2>;

    Float a = Float(2, { 1, 0 });
    Float b = Float(4, { 0, 2 });
    Float c = a + b;
    REQUIRE( c.V == 6 );
    REQUIRE( c.dVd(0) == 1 );
    REQUIRE( c.dVd(1) == 2 );
}

TEST_CASE( "Autodiff division", "[autodiff]" ) {
    using Float = FADFloat<float, 2>;

    Float a = Float(4, { 1, 0 });
    Float b = Float(2, { 0, 1 });
    Float c = a / b;
    REQUIRE( c.V == 2 );
    REQUIRE( c.dVd(0) == 0.5f );
    REQUIRE( c.dVd(1) == -1 );
}

TEST_CASE( "Autodiff nested", "[autodiff]" ) {
    using Float = FADFloat<float, 2>;

    Float a = Float(4, { 1, 0 });
    Float b = Float(2, { 0, 1 });
    Float c = Float(5, { 0, 0 });
    Float d = (a - b) * (a + b) - c;
    REQUIRE( d.V == 7 );
    REQUIRE( d.dVd(0) == 8 );
    REQUIRE( d.dVd(1) == -4 );
}

TEST_CASE( "Autodiff vector length", "[autodiff]" ) {
    using Float = FADFloat<float, 1>;
    using Vector = lore::Vector<Float, 3>;

    Vector x {
        Float { 3, 1 },
        Float { 0, 0 },
        Float { -4, 0 }
    };
    Float len = x.length();

    REQUIRE( len.V == 5 );
    REQUIRE( len.dVd(0) == 0.6f );
}

template<typename Float>
Float f(Float x, Float y) {
    return (x + Float(1)) * y;
}

TEST_CASE( "Autodiff functions", "[autodiff]" ) {
    using Float = FADFloat<float, 1>;

    Float x { 10, 1 };
    Float y { 5, 0 };

    Float result = f(x, y);

    REQUIRE( result.V == 55 );
    REQUIRE( result.dVd(0) == 5 );
}
