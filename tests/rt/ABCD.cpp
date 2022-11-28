#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/Surface.h>
#include <lore/rt/ABCD.h>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Ray transfer matrices", "[rt]" ) {
    Lens<float> lens;
    lens.surfaces.emplace_back(
            60.f,
            5.f,
            20.f,
            true,
            Glass<float>::constant(1.7f)
    );
    lens.surfaces.emplace_back(
            -500.f,
            70.f,
            20.f,
            true,
            Glass<float>::air()
    );

    const auto result = abcd::full(lens, 0.58f);
    REQUIRE_THAT( result(0, 0), WithinRel(0.0543823, 1e-5) );
    REQUIRE_THAT( result(0, 1), WithinRel(72.6529, 1e-5) );
    REQUIRE_THAT( result(1, 0), WithinRel(-0.0130186, 1e-5) );
    REQUIRE_THAT( result(1, 1), WithinRel(0.995882, 1e-5) );
}
