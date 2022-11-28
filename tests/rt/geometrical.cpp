#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/Surface.h>
#include <lore/rt/geometrical.h>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Intersections for positive radii", "[rt]" ) {
    Surface<float> surface {};
    surface.radius = 13.f;
    surface.aperture = 0.f;
    surface.checkAperture = false;
    surface.thickness = 0.f;
    surface.glass = lore::Glass<float>::air();

    SECTION( "On-axis ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, -10);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "On-axis inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, 10);
        ray.direction = Vector3<float>(0, 0, -1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "Off-axis ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(3, 4, -12);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 13 );
    }

    SECTION( "Off-axis inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(3, 4, 14);
        ray.direction = Vector3<float>(0, 0, -1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 13 );
    }

    SECTION( "Skew ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, -1, -1).normalized() * 10;
        ray.direction = Vector3<float>(0, 1, 1).normalized();

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE_THAT( t, WithinAbs(10.f, 1e-5) );
    }

    SECTION( "Skew inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, -1, 1).normalized() * 10;
        ray.direction = Vector3<float>(0, 1, -1).normalized();

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE_THAT( t, WithinAbs(10.f, 1e-5) );
    }

    SECTION( "Missing ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(10, 10, -10);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == false );
    }
}

TEST_CASE( "Intersections for negative radii", "[rt]" ) {
    Surface<float> surface;
    surface.radius = -13.f;
    surface.aperture = 0.f;
    surface.checkAperture = false;
    surface.thickness = 0.f;
    surface.glass = lore::Glass<float>::air();

    SECTION( "On-axis ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, -10);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "On-axis inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, 10);
        ray.direction = Vector3<float>(0, 0, -1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "Off-axis ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(3, 4, -14);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 13 );
    }

    SECTION( "Off-axis inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(3, 4, 12);
        ray.direction = Vector3<float>(0, 0, -1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 13 );
    }

    SECTION( "Skew ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, -1, -1).normalized() * 10;
        ray.direction = Vector3<float>(0, 1, 1).normalized();

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE_THAT( t, WithinAbs(10.f, 1e-5) );
    }

    SECTION( "Skew inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, -1, 1).normalized() * 10;
        ray.direction = Vector3<float>(0, 1, -1).normalized();

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE_THAT( t, WithinAbs(10.f, 1e-5) );
    }

    SECTION( "Missing ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(10, 10, -10);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == false );
    }
}

TEST_CASE( "Intersections for flat surfaces", "[rt]" ) {
    Surface<float> surface;
    surface.radius = 0.f;
    surface.aperture = 0.f;
    surface.checkAperture = false;
    surface.thickness = 0.f;
    surface.glass = lore::Glass<float>::air();

    SECTION( "On-axis ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, -10);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "On-axis inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, 10);
        ray.direction = Vector3<float>(0, 0, -1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "Off-axis ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(3, 4, -10);
        ray.direction = Vector3<float>(0, 0, 1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "Off-axis inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(3, 4, 10);
        ray.direction = Vector3<float>(0, 0, -1);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE( t == 10 );
    }

    SECTION( "Skew ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 7, -20/sqrt(2.f));
        ray.direction = Vector3<float>(0, 1, 1).normalized();

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE_THAT( t, WithinAbs(20.f, 1e-5) );
    }

    SECTION( "Skew inverse ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 7, 20/sqrt(2.f));
        ray.direction = Vector3<float>(0, 1, -1).normalized();

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == true );
        REQUIRE_THAT( t, WithinAbs(20.f, 1e-5) );
    }

    SECTION( "Missing ray" ) {
        rt::Ray<float> ray;
        ray.origin = Vector3<float>(0, 0, 1);
        ray.direction = Vector3<float>(0, 1, 0);

        float t;
        REQUIRE( rt::geometrical(ray, surface, t) == false );
    }
}
