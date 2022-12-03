#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lore/lore.h>
#include <lore/lens/Lens.h>
#include <lore/io/LensReader.h>
#include <lore/rt/GeometricalIntersector.h>
#include <lore/rt/SequentialTrace.h>
#include <lore/optim/FADFloat.h>

#include <fstream>

using namespace lore;
using namespace Catch::Matchers;

TEST_CASE( "Sequential tracing", "[rt]" ) {
    //using Float = double;
    using Float = optim::FADFloat<double, 1>;

    io::LensReader reader;
    std::ifstream file("data/lenses/simple.len");
    auto result = reader.read(file);
    auto config = result.front();
    auto lens = config.lens<Float>();

    rt::SequentialTrace<Float> seq { 0.587560 };
    rt::GeometricalIntersector<Float> intersector {};

    for (const Float &relField : (Float[]){ 0, 0.7, 1 }) {
        const Float field = relField * lens.surfaces.front().aperture;
        // measured in angles for conjugate distances >= 1e+8

        Vector3<Float> referenceOrigin = { 0, -field, 0 };
        const Vector3<Float> referenceAim = { 0, 0, lens.surfaces.front().thickness };
        const Float a = 1e-14;
        referenceOrigin = referenceOrigin * a + referenceAim * (Float(1) - a);

        rt::Ray<Float> referenceRay {
            referenceOrigin,
            (referenceAim - referenceOrigin).normalized()
        };

        if (!seq(referenceRay, lens, intersector)) {
            std::cerr << "reference ray failed for field " << std::to_string(detach(relField)) << std::endl;
            continue;
        }

        std::ofstream dump {"tangential-" + std::to_string(detach(relField)) + ".bin"};
        const int r = ceil(config.entranceBeamRadius);
        for (int iy = -r; iy <= +r; iy++) {
            const auto y = Float(iy, 1);
            const double yV = detach(y);
            dump.write((char *)&yV, sizeof(yV));
            for (const auto &ww: config.wavelengths) {
                seq.wavelength = ww.wavelength;

                Vector3<Float> origin = { 0, -field, 0 };
                const Vector3<Float> aim = { 0, y, lens.surfaces.front().thickness };
                origin = origin * a + aim * (Float(1) - a);
                rt::Ray<Float> ray {
                    origin,
                    (aim - origin).normalized()
                };
                if (!seq(ray, lens, intersector)) {
                    ray.origin.y() = NAN;
                }

                const Float dy = ray.origin.y() - referenceRay.origin.y();
                const double dyV = detach(dy);
                const double dyD = dy.dVd(0);
                dump.write((char *)&dyV, sizeof(dyV));
                dump.write((char *)&dyD, sizeof(dyD));
            }
        }

        std::ofstream dump2 {"sagittal-" + std::to_string(detach(relField)) + ".bin"};
        for (int iy = 0; iy <= +r; iy++) {
            const auto y = Float(iy, 1);
            const double yV = detach(y);
            dump2.write((char *)&yV, sizeof(yV));
            for (const auto &ww: config.wavelengths) {
                seq.wavelength = ww.wavelength;

                Vector3<Float> origin = { 0, -field, 0 };
                const Vector3<Float> aim = { y, 0, lens.surfaces.front().thickness };
                origin = origin * a + aim * (Float(1) - a);
                rt::Ray<Float> ray {
                    origin,
                    (aim - origin).normalized()
                };
                if (!seq(ray, lens, intersector)) {
                    ray.origin.x() = NAN;
                }

                const Float dy = ray.origin.x();
                const double dyV = detach(dy);
                const double dyD = dy.dVd(0);
                dump2.write((char *)&dyV, sizeof(dyV));
                dump2.write((char *)&dyD, sizeof(dyD));
            }
        }
    }
}

TEST_CASE( "Inverse sequential tracing", "[rt]" ) {
    using Float = double;

    GlassCatalog::shared.read("data/glass/schott.glc");
    GlassCatalog::shared.read("data/glass/obsolete001.glc");

    io::LensReader reader;
    std::ifstream file("data/lenses/dgauss.len");
    auto result = reader.read(file);
    auto config = result.front();
    auto lens = config.lens<Float>();

    rt::InverseSequentialTrace<Float> trace { 0.587560 };
    rt::GeometricalIntersector<Float> intersector {};

    rt::Ray<Float> ray;
    ray.origin = { 0, 0, 0 };
    ray.direction = Vector3<Float>(0, -0.55, -1).normalized();

    REQUIRE( trace(ray, lens, intersector) == true );
    REQUIRE_THAT( ray.direction.z(), WithinAbs(-1, 1e-5) );
    REQUIRE_THAT( ray.origin.x(), WithinAbs(0, 1e-5) );
    REQUIRE_THAT( ray.origin.y(), WithinAbs(-48.0411, 1e-5) );
    REQUIRE_THAT( ray.origin.z(), WithinAbs(6.87744, 1e-5) );
}
