#pragma once

#include "Knots.h"

#include <vector>
#include <stdexcept>

namespace ANurbs {

template <typename TScalar, typename TVector>
class SurfaceGeometryBase
{
public:
    using ScalarType = TScalar;
    using VectorType = TVector;
    using KnotsType = std::vector<ScalarType>;

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
        const int& nbPolesV
    )
    : m_degreeU(degreeU)
    , m_degreeV(degreeV)
    , m_knotsU(nbPolesU + degreeU - 1)
    , m_knotsV(nbPolesV + degreeV - 1)
    {
    }

    static constexpr int
    Dimension(
    )
    {
        return VectorMath<VectorType>::Dimension();
    }

    int
    DegreeU(
    ) const
    {
        return m_degreeU;
    }

    int
    DegreeV(
    ) const
    {
        return m_degreeV;
    }

    template <int Index>
    int
    NbKnots(
    ) const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Index)
        {
            case 0:
                return m_knotsU.size();
            case 1:
                return m_knotsV.size();
        }
    }

    template <int Axis>
    ScalarType
    Knot(
        const int& index
    ) const
    {
        static_assert(Axis == 0 || Axis == 1, "Invalid index");

        switch (Axis)
        {
            case 0:
                return m_knotsU[index];
            case 1:
                return m_knotsV[index];
        }
    }

    int
    NbKnotsU(
    ) const
    {
        return m_knotsU.size();
    }

    ScalarType&
    KnotU(
        const int& index
    )
    {
        return m_knotsU[index];
    }

    void
    SetKnotU(
        const int& index,
        const ScalarType& value
    )
    {
        m_knotsU[index] = value;
    }

    const KnotsType&
    KnotsU(
    ) const
    {
        return m_knotsU;
    }

    int
    NbKnotsV(
    ) const
    {
        return m_knotsV.size();
    }

    ScalarType&
    KnotV(
        const int& index
    )
    {
        return m_knotsV[index];
    }

    void
    SetKnotV(
        const int& index,
        const ScalarType& value
    )
    {
        m_knotsV[index] = value;
    }

    const KnotsType&
    KnotsV(
    ) const
    {
        return m_knotsV;
    }

    int
    NbPolesU(
    ) const
    {
        return NbKnotsU() - DegreeU() + 1;
    }

    int
    NbPolesV(
    ) const
    {
        return NbKnotsV() - DegreeV() + 1;
    }

    int
    NbPoles(
    ) const
    {
        return NbPolesU() * NbPolesV();
    }

    virtual VectorType
    Pole(
        const int& indexU,
        const int& indexV
    ) const = 0;

    virtual void
    SetPole(
        const int& indexU,
        const int& indexV,
        const VectorType& value
    ) = 0;

    virtual ScalarType
    Weight(
        const int& indexU,
        const int& indexV
    ) const = 0;

    virtual void
    SetWeight(
        const int& indexU,
        const int& indexV,
        const ScalarType& value
    ) = 0

    virtual bool
    IsRational(
    ) const = 0;

    template <typename TValue>
    TValue
    EvaluateAt(
        std::function<TValue(int, int)> getValue,
        const ScalarType& u,
        const ScalarType& v
    ) const
    {
        // compute shape functions

        SurfaceShapeEval<ScalarType> shape(DegreeU(), DegreeV(), 0);

        if (IsRational()) {
            shape.compute(KnotsU(), KnotsV(), [&](int i, int j) -> ScalarType {
                return Weight(i, j);}, u, v);
        } else {
            shape.compute(KnotsU(), KnotsV(), u, v);
        }

        // compute value

        TValue value;

        for (int i = 0; i <= degreeU(); i++) {
            for (int j = 0; j <= degreeV(); j++) {
                int poleU = firstNonzeroPoleU() + i;
                int poleV = firstNonzeroPoleV() + j;

                TValue v = getValue(poleU, poleV) * value(derivative, i, j);

                if (value == 0) {
                    value = v;
                } else {
                    value += v;
                }
            }
        }

        return value;
    }

    template <typename TValue>
    std::vector<TValue>
    EvaluateAt(
        std::function<TValue(int, int)> getValue,
        const ScalarType& u,
        const ScalarType& v,
        const int& order
    ) const
    {
        // compute shape functions

        SurfaceShapeEval<ScalarType> shape(DegreeU(), DegreeV(), order);
        
        if (IsRational()) {
            shape.compute(KnotsU(), KnotsV(), [&](int i, int j) -> ScalarType {
                return Weight(i, j);}, u, v);
        } else {
            shape.compute(KnotsU(), KnotsV(), u, v);
        }

        // compute derivatives

        int nbShapes = NbShapes(order);

        std::vector<TVector> values(nbShapes);

        for (int k = 0; k < nbShapes; k++) {
            for (int i = 0; i <= degreeU(); i++) {
                for (int j = 0; j <= degreeV(); j++) {
                    int poleU = firstNonzeroPoleU() + i;
                    int poleV = firstNonzeroPoleV() + j;

                    TValue v = getValue(poleU, poleV) * value(derivative, i, j);

                    if (value == 0) {
                        values[k] = v;
                    } else {
                        values[k] += v;
                    }
                }
            }
        }

        return values;
    }

    VectorType
    PointAt(
        const ScalarType& u,
        const ScalarType& v
    )
    {
        return EvaluateAt<VectorType>([&](int i, int j) -> VectorType {
            return Pole(i, j);}, u, v);
    }

    std::vector<VectorType>
    DerivativesAt(
        const ScalarType& u,
        const ScalarType& v,
        const int& order
    ) const
    {
        return EvaluateAt<VectorType>([&](int i, int j) -> VectorType {
            return Pole(i, j);}, u, v, order);
    }
};

} // namespace ANurbs
