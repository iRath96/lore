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
    std::ifstream file("data/lenses/tessar.len");
    auto result = reader.read(file);
    auto config = result.front();
    auto lens = config.lens<Float>();

    rt::SequentialTrace<Float> seq { 0.587560 };
    rt::GeometricalIntersector<Float> intersector {};

    for (const Float &relField : (Float[]){ 0, 0.7, 1 }) {
        const Float field = relField * lens.surfaces.front().aperture;
        // measured in angles for conjugate distances >= 1e+8

        const Vector3<Float> referenceOrigin = { 0, -field, 0 };
        const Vector3<Float> referenceAim = { 0, 0, lens.surfaces.front().thickness };
        rt::Ray<Float> referenceRay {
            referenceOrigin,
            (referenceAim - referenceOrigin).normalized()
        };

        if (!seq(referenceRay, lens, intersector)) {
            continue;
        }

        std::ofstream dump {"tangential-" + std::to_string(relField) + ".bin"};
        const int r = ceil(config.entranceBeamRadius);
        for (int iy = -r; iy <= +r; iy++) {
            const auto y = Float(iy);
            dump.write((char *) &y, sizeof(Float));
            for (const auto &ww: config.wavelengths) {
                seq.wavelength = ww.wavelength;

                const Vector3<Float> origin = { 0, -field, 0 };
                const Vector3<Float> aim = { 0, y, lens.surfaces.front().thickness };
                rt::Ray<Float> ray {
                    origin,
                    (aim - origin).normalized()
                };
                if (!seq(ray, lens, intersector)) {
                    ray.origin.y() = NAN;
                }
                Float dy = ray.origin.y() - referenceRay.origin.y();
                dump.write((char *) &dy, sizeof(Float));
            }
        }

        std::ofstream dump2 {"sagittal-" + std::to_string(relField) + ".bin"};
        for (int iy = 0; iy <= +r; iy++) {
            const auto y = Float(iy);
            dump2.write((char *) &y, sizeof(Float));
            for (const auto &ww: config.wavelengths) {
                seq.wavelength = ww.wavelength;

                const Vector3<Float> origin = { 0, -field, 0 };
                const Vector3<Float> aim = { y, 0, lens.surfaces.front().thickness };
                rt::Ray<Float> ray {
                    origin,
                    (aim - origin).normalized()
                };
                if (!seq(ray, lens, intersector)) {
                    ray.origin.x() = NAN;
                }
                dump2.write((char *) &ray.origin.x(), sizeof(Float));
            }
        }
    }
}
