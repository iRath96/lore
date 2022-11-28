#include <catch2/catch_test_macros.hpp>

#include <lore/lore.h>
#include <lore/io/LensReader.h>

#include <fstream>

TEST_CASE( "Lenses are read correctly", "[io]" ) {
    std::ifstream file("data/lenses/tessar.len");

    lore::io::LensReader reader;
    auto result = reader.read(file);

    REQUIRE( result.size() == 1 );

    const auto &lens = result.front();

    SECTION( "Description" ) {
        REQUIRE( lens.name == "No name" );
        REQUIRE( lens.description == "OSLO" );
    }

    SECTION( "Wavelengths" ) {
        REQUIRE( lens.wavelengths.size() == 3 );
        REQUIRE( lens.wavelengths[0].wavelength == 0.58756f );
        REQUIRE( lens.wavelengths[1].wavelength == 0.48613f );
        REQUIRE( lens.wavelengths[2].wavelength == 0.65627f );
        REQUIRE( lens.wavelengths[0].weight == 1.0f );
        REQUIRE( lens.wavelengths[1].weight == 1.0f );
        REQUIRE( lens.wavelengths[2].weight == 1.0f );
    }

    SECTION( "Surfaces" ) {
        REQUIRE( lens.surfaces.size() == 10 );
        REQUIRE( lens.surfaces[0].thickness == float(1e+20) );
        REQUIRE( lens.surfaces[1].thickness == 7.5f );
        REQUIRE( lens.surfaces[2].thickness == 4.6f );

        REQUIRE( lens.surfaces[1].aperture == 18.0f );
        REQUIRE( lens.surfaces[2].aperture == 18.0f );
        REQUIRE( lens.surfaces[3].aperture == 15.8f );

        REQUIRE( lens.surfaces[0].checkAperture == false );
        REQUIRE( lens.surfaces[7].checkAperture == true );

        REQUIRE( lens.surfaces[1].radius == 37.3f );
        REQUIRE( lens.surfaces[4].radius == -83.8f );
    }
}
