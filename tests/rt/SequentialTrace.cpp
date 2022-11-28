#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/Lens.h>
#include <lore/io/LensReader.h>
#include <lore/rt/GeometricalIntersector.h>
#include <lore/rt/SequentialTrace.h>

#include <fstream>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Sequential tracing", "[rt]" ) {
    using Float = double;

    io::LensReader reader;
    std::ifstream file("data/lenses/simple.len");
    auto result = reader.read(file);
    auto config = result.front();
    auto lens = config.lens<Float>();

    rt::SequentialTrace<Float> seq { 0.587560 };
    rt::GeometricalIntersector<Float> intersector {};

    std::ofstream dump { "test1.bin" };
    const int r = ceil(config.entranceBeamRadius);
    for (int iy = -r; iy <= +r; iy++) {
        const Float y = Float(iy);
        dump.write((char *) &y, sizeof(Float));
        for (const auto &ww : config.wavelengths) {
            seq.wavelength = ww.wavelength;

            rt::Ray<Float> ray {
                {0, y, -10},
                {0, 0, 1}
            };
            if (!seq(ray, lens, intersector)) {
                ray.origin.y() = NAN;
            }
            dump.write((char *) &ray.origin.y(), sizeof(Float));
        }
    }

    std::ofstream dump2 { "test2.bin" };
    for (int iy = -r; iy <= +r; iy++) {
        const Float y = Float(iy);
        dump2.write((char *) &y, sizeof(Float));
        for (const auto &ww : config.wavelengths) {
            seq.wavelength = ww.wavelength;

            rt::Ray<Float> ray {
                {y, 0, -10},
                {0, 0, 1}
            };
            if (!seq(ray, lens, intersector)) {
                ray.origin.x() = NAN;
            }
            dump2.write((char *) &ray.origin.x(), sizeof(Float));
        }
    }
}
