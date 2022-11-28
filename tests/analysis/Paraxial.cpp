#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/Surface.h>
#include <lore/lens/Lens.h>
#include <lore/analysis/Paraxial.h>

#include <iostream>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Paraxial analysis", "[analysis]" ) {
    SECTION( "Effective Focal Length" ) {
        using Float = float;

        Lens<Float> lens;
        lens.surfaces.emplace_back(
            Float(60),
            Float(5),
            Float(20),
            true,
            Glass<Float>::constant(Float(1.7))
        );
        lens.surfaces.emplace_back(
            Float(-500),
            Float(70),
            Float(20),
            true,
            Glass<Float>::air()
        );

        const auto analysis = ParaxialAnalysis(lens, Float(0.58));
        REQUIRE_THAT( analysis.efl, WithinRel(76.8130130281, 1e-5) );
        REQUIRE_THAT( analysis.focalShift, WithinRel(4.1772723850, 1e-5) );
    }
}
