#pragma once

#include "CurveGeometryBase.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

template <typename TScalar, int TDimension,
    typename TVector = Point<TScalar, TDimension>>
class CurveGeometry
    : public CurveGeometryBase<TScalar, TVector>
{
public:
    using CurveGeometryBaseType = CurveGeometryBase<TScalar, TVector>;
    using typename CurveGeometryBaseType::KnotsType;
    using typename CurveGeometryBaseType::ScalarType;
    using typename CurveGeometryBaseType::VectorType;

protected:
    std::vector<VectorType> m_poles;
    std::vector<ScalarType> m_weights;

public:
    CurveGeometry(
        const int& degree,
        const int& nbPoles,
        const bool& isRational)
        : CurveGeometryBaseType(degree, nbPoles)
        , m_poles(nbPoles)
        , m_weights(isRational ? nbPoles : 0)
    {
    }

    VectorType
    Pole(
        const int& index) const override
    {
        return m_poles[index];
    }

    void
    SetPole(
        const int& index,
        const VectorType& value) override
    {
        m_poles[index] = value;
    }

    bool
    IsRational() const override
    {
        return m_weights.size() != 0;
    }

    ScalarType
    Weight(
        const int& index) const override
    {
        if (IsRational()) {
            return m_weights[index];
        } else {
            return 1;
        }
    }

    void
    SetWeight(
        const int& index,
        const ScalarType& value) override
    {
        if (IsRational()) {
            m_weights[index] = value;
        } else {
            throw std::invalid_argument("Geometry is not rational");
        }
    }
};

using CurveGeometry1D = CurveGeometry<double, 1>;
using CurveGeometry2D = CurveGeometry<double, 2>;
using CurveGeometry3D = CurveGeometry<double, 3>;

} // namespace ANurbs
