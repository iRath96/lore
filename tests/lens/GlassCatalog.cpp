#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/GlassCatalog.h>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Reading glass catalogs", "[autodiff]" ) {
    SECTION( "Schott" ) {
        GlassCatalog catalog;
        std::ifstream file { "data/glass/schott.glc" };
        REQUIRE( catalog.read(file) == 141 );
        REQUIRE_THAT( catalog.glass("N-BK7").ior(0.38f), WithinAbs(1.53374f, 1e-5) );
    }

    SECTION( "Obsolete 001" ) {
        GlassCatalog catalog;
        std::ifstream file { "data/glass/obsolete001.glc" };
        REQUIRE( catalog.read(file) == 671 );
        REQUIRE_THAT( catalog.glass("H_BACD6").ior(0.58f), WithinAbs(1.61395f, 1e-5) );
    }
}
