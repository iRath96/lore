#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/Surface.h>
#include <lore/rt/geometrical.h>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Vector math", "[math]" ) {
    SECTION( "Dot product" ) {
        Vector3<float> a(1, 2, 3);
        Vector3<float> b(5, -1, 0);
        REQUIRE( a.dot(b) == 3 );
    }

    SECTION( "Squared length" ) {
        Vector3<float> a(3, 4, 12);
        REQUIRE( a.lengthSquared() == 169 );
    }

    SECTION( "Length" ) {
        Vector3<float> a(3, 4, 12);
        REQUIRE( a.length() == 13 );
    }

    SECTION( "Normalization" ) {
        Vector3<float> a(-3, 0, 4);
        Vector3<float> b = a.normalized();

        REQUIRE( b.x() == -0.6f );
        REQUIRE( b.y() == 0.f );
        REQUIRE( b.z() == 0.8f );
    }
}

TEST_CASE( "Matrix math", "[math]" ) {
    SECTION( "Product" ) {
        Matrix2x2<float> a(
                1, 2,
                3, 4
                );
        Matrix2x2<float> b(
                2, 0,
                1, 2
                );
        Matrix2x2<float> expected(
                4, 4,
                10, 8
                );
        REQUIRE( a * b == expected );
    }

    SECTION( "Squared length" ) {
        Vector3<float> a(3, 4, 12);
        REQUIRE( a.lengthSquared() == 169 );
    }

    SECTION( "Length" ) {
        Vector3<float> a(3, 4, 12);
        REQUIRE( a.length() == 13 );
    }

    SECTION( "Normalization" ) {
        Vector3<float> a(-3, 0, 4);
        Vector3<float> b = a.normalized();

        REQUIRE( b.x() == -0.6f );
        REQUIRE( b.y() == 0.f );
        REQUIRE( b.z() == 0.8f );
    }
}
