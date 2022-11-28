#include <lore/lore.h>
#include <lore/math.h>

namespace lore::rt {

template<typename Float>
struct Ray {
    Vector3<Float> origin;
    Vector3<Float> direction;

    Vector3<Float> operator()(Float t) const {
        return origin + t * direction;
    }
};

}
