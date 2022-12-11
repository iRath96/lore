#pragma once

#include <lore/lore.h>
#include <lore/math.h>

namespace lore {

template<int N, int M, typename Float = float>
struct LaurentIOR {
    Float A[N + M];

    LaurentIOR() {}
    LaurentIOR(const MTL_THREAD Float (&A)[N + M]) {
        for (int i = 0; i < N + M; i++) {
            this->A[i] = A[i];
        }
    }

    /**
     * Computes the Sellmeier equation for a given wavelength in micrometers.
     */
    Float operator()(Float wavelength) const MTL_DEVICE {
        const Float wSqr = sqr(wavelength);
        Float nSqr = 0;

        Float multiplier = 1;
        for (int i = 0; i < N; i++) {
            nSqr += A[i] * multiplier;
            multiplier *= wSqr;
        }

        const Float step = Float(1) / wSqr;
        multiplier = step;

        for (int i = 0; i < M; i++) {
            nSqr += A[N+i] * multiplier;
            multiplier *= step;
        }

        return sqrt(nSqr);
    }
    

    static LaurentIOR constantIOR(Float ior) {
        LaurentIOR result;
        result.A[0] = ior;
        for (int i = 1; i < N+M; i++) {
            result.A[i] = 0;
        }
        return result;
    }

    static LaurentIOR air() {
        return constantIOR(1);
    }

    bool isAir() const {
        return *this == air();
    }

    bool operator==(const MTL_DEVICE LaurentIOR &other) const MTL_DEVICE {
        for (int i = 0; i < N+M; i++) {
            if (A[i] != other.A[i]) {
                return false;
            }
        }
        return true;
    }

    template<typename LFloat>
    LaurentIOR<N, M, LFloat> cast() const {
        LaurentIOR<N, M, LFloat> result;
        for (int i = 0; i < N+M; i++) {
            result.A[i] = LFloat(A[i]);
        }
        return result;
    }
};

template<int N, typename Float = float>
struct SellmeierIOR {
    Float B[N];
    Float C[N];

    SellmeierIOR() {}
    SellmeierIOR(const MTL_THREAD Float (&B)[N], const MTL_THREAD Float (&C)[N]) {
        for (int i = 0; i < N; i++) {
            this->B[i] = B[i];
            this->C[i] = C[i];
        }
    }

    /**
     * Computes the Sellmeier equation for a given wavelength in micrometers.
     */
    Float operator()(Float wavelength) const MTL_DEVICE {
        const Float wSqr = sqr(wavelength);
        Float nSqr = 1;
        for (int i = 0; i < N; i++) {
            nSqr += B[i] * wSqr / (wSqr - C[i]);
        }
        return sqrt(nSqr);
    }

    static SellmeierIOR air() {
        SellmeierIOR result;
        for (int i = 0; i < N; i++) {
            result.B[i] = 0;
            result.C[i] = 0;
        }
        return result;
    }

    static SellmeierIOR constantIOR(Float ior) {
        SellmeierIOR result = air();
        result.B[0] = sqr(ior) - 1;
        return result;
    }

    bool isAir() const {
        return *this == air();
    }

    bool operator==(const MTL_DEVICE SellmeierIOR &other) const MTL_DEVICE {
        for (int i = 0; i < N; i++) {
            if (B[i] != other.B[i]) {
                return false;
            }
            if (C[i] != other.C[i]) {
                return false;
            }
        }
        return true;
    }

    template<typename LFloat>
    SellmeierIOR<N, LFloat> cast() const {
        SellmeierIOR<N, LFloat> result;
        for (int i = 0; i < N; i++) {
            result.B[i] = LFloat(B[i]);
            result.C[i] = LFloat(C[i]);
        }
        return result;
    }
};

template<typename Float = float>
struct Glass {
    enum IORType {
        SCHOTT2X4,
        SELL3T
    };

    IORType type;

    union {
        SellmeierIOR<3, Float> sell3t;
        LaurentIOR<2, 4, Float> schott2x4;
    };

    Glass() : type(SELL3T), sell3t(sell3t.air()) {}
    explicit Glass(MTL_THREAD const SellmeierIOR<3, Float> &ior) : type(SELL3T), sell3t(ior) {}
    explicit Glass(MTL_THREAD const LaurentIOR<2, 4, Float> &ior) : type(SCHOTT2X4), schott2x4(ior) {}

    Float ior(Float wavelength) const MTL_DEVICE {
        switch (type) {
            case SELL3T: return sell3t(wavelength);
            case SCHOTT2X4: return schott2x4(wavelength);
        }
    }

    static Glass air() {
        return Glass(SellmeierIOR<3, Float>::air());
    }

    static Glass constantIOR(Float ior) {
        return Glass(SellmeierIOR<3, Float>::constantIOR(ior));
    }

    bool isAir() const {
        switch (type) {
            case SELL3T: return sell3t.isAir();
            case SCHOTT2X4: return schott2x4.isAir();
        }
    }

    template<typename LFloat>
    Glass<LFloat> cast() const {
        switch (type) {
            case SELL3T: return Glass<LFloat>(sell3t.template cast<LFloat>());
            case SCHOTT2X4: return Glass<LFloat>(schott2x4.template cast<LFloat>());
        }
    }
};

}
