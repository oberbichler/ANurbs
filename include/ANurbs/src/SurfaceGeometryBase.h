#pragma once

#include "Interval.h"
#include "Knots.h"
#include "SurfaceShapeEvaluator.h"
#include "VectorMath.h"

#include <vector>

namespace ANurbs {

template <typename TScalar, typename TVector>
class SurfaceGeometryBase
{
public:
    using ScalarType = TScalar;
    using VectorType = TVector;
    using KnotsType = std::vector<ScalarType>;
    using IntervalType = Interval<ScalarType>;

protected:
    int m_degreeU;
    int m_degreeV;
    KnotsType m_knotsU;
    KnotsType m_knotsV;

public:
    SurfaceGeometryBase(
        const int& degreeU,
        const int& degreeV,
        const int& nbPolesU,
        const int& nbPolesV)
        : m_degreeU(degreeU)
        , m_degreeV(degreeV)
        , m_knotsU(nbPolesU + degreeU - 1)
        , m_knotsV(nbPolesV + degreeV - 1)
    {
    }

    static constexpr int
    Dimension()
    {
        return VectorMath<VectorType>::Dimension();
    }

    int
    DegreeU() const
    {
        return m_degreeU;
    }

    int
    DegreeV() const
    {
        return m_degreeV;
    }

    IntervalType
    DomainU()
    {
        ScalarType u0 = KnotU(DegreeU() - 1);
        ScalarType u1 = KnotU(NbKnotsU() - DegreeU());

        return IntervalType(u0, u1);
    }

    IntervalType
    DomainV()
    {
        ScalarType v0 = KnotV(DegreeV() - 1);
        ScalarType v1 = KnotV(NbKnotsV() - DegreeV());

        return IntervalType(v0, v1);
    }

    template <int Axis>
    int
    NbKnots() const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis) {
        case 0:
            return NbKnotsU();
        case 1:
            return NbKnotsV();
        }
    }

    template <int Axis>
    ScalarType
    Knot(
        const int& index) const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis) {
        case 0:
            return KnotU[index];
        case 1:
            return KnotV[index];
        }
    }

    int
    NbKnotsU() const
    {
        return static_cast<int>(m_knotsU.size());
    }

    ScalarType&
    KnotU(
        const int& index)
    {
        return m_knotsU[index];
    }

    void
    SetKnotU(
        const int& index,
        const ScalarType& value)
    {
        m_knotsU[index] = value;
    }

    const KnotsType&
    KnotsU() const
    {
        return m_knotsU;
    }

    int
    NbKnotsV() const
    {
        return static_cast<int>(m_knotsV.size());
    }

    ScalarType&
    KnotV(
        const int& index)
    {
        return m_knotsV[index];
    }

    void
    SetKnotV(
        const int& index,
        const ScalarType& value)
    {
        m_knotsV[index] = value;
    }

    const KnotsType&
    KnotsV() const
    {
        return m_knotsV;
    }

    int
    NbPolesU() const
    {
        return NbKnotsU() - DegreeU() + 1;
    }

    int
    NbPolesV() const
    {
        return NbKnotsV() - DegreeV() + 1;
    }

    int
    NbPoles() const
    {
        return NbPolesU() * NbPolesV();
    }

    virtual VectorType
    Pole(
        const int& indexU,
        const int& indexV) const = 0;

    virtual void
    SetPole(
        const int& indexU,
        const int& indexV,
        const VectorType& value)
        = 0;

    virtual ScalarType
    Weight(
        const int& indexU,
        const int& indexV) const = 0;

    virtual void
    SetWeight(
        const int& indexU,
        const int& indexV,
        const ScalarType& value)
        = 0;

    virtual bool
    IsRational() const = 0;

    template <typename TValue, typename TValues>
    TValue
    EvaluateAt(
        TValues values,
        const ScalarType& u,
        const ScalarType& v) const
    {
        // compute shape functions

        SurfaceShapeEvaluator<ScalarType> shape(DegreeU(), DegreeV(), 0);

        if (IsRational()) {
            shape.Compute(KnotsU(), KnotsV(), [&](int i, int j) -> ScalarType { return Weight(i, j); }, u, v);
        } else {
            shape.Compute(KnotsU(), KnotsV(), u, v);
        }

        // compute value

        TValue result;

        for (int i = 0; i <= DegreeU(); i++) {
            for (int j = 0; j <= DegreeV(); j++) {
                int poleU = shape.FirstNonzeroPoleU() + i;
                int poleV = shape.FirstNonzeroPoleV() + j;

                TValue value = values(poleU, poleV) * shape(0, i, j);

                if (i == 0 && j == 0) {
                    result = value;
                } else {
                    result += value;
                }
            }
        }

        return result;
    }

    template <typename TValue, typename TValues>
    std::vector<TValue>
    EvaluateAt(
        TValues values,
        const ScalarType& u,
        const ScalarType& v,
        const int& order) const
    {
        // compute shape functions

        SurfaceShapeEvaluator<ScalarType> shape(DegreeU(), DegreeV(), order);

        if (IsRational()) {
            shape.Compute(KnotsU(), KnotsV(), [&](int i, int j) -> ScalarType { return Weight(i, j); }, u, v);
        } else {
            shape.Compute(KnotsU(), KnotsV(), u, v);
        }

        // compute derivatives

        int nbShapes = shape.NbShapes(order);

        std::vector<TValue> result(nbShapes);

        for (int k = 0; k < nbShapes; k++) {
            for (int i = 0; i <= DegreeU(); i++) {
                for (int j = 0; j <= DegreeV(); j++) {
                    int poleU = shape.FirstNonzeroPoleU() + i;
                    int poleV = shape.FirstNonzeroPoleV() + j;

                    TValue value = values(poleU, poleV) * shape(k, i, j);

                    if (i == 0 && j == 0) {
                        result[k] = value;
                    } else {
                        result[k] += value;
                    }
                }
            }
        }

        return result;
    }

    VectorType
    PointAt(
        const ScalarType& u,
        const ScalarType& v)
    {
        auto poles = [&](int i, int j) -> VectorType { return Pole(i, j); };

        return EvaluateAt<VectorType>(poles, u, v);
    }

    std::vector<VectorType>
    DerivativesAt(
        const ScalarType& u,
        const ScalarType& v,
        const int& order) const
    {
        auto poles = [&](int i, int j) -> VectorType { return Pole(i, j); };

        return EvaluateAt<VectorType>(poles, u, v, order);
    }
};

} // namespace ANurbs
