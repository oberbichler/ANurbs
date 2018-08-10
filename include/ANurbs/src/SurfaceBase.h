#pragma once

#include "Interval.h"
#include "VectorMath.h"

#include <vector>

namespace ANurbs {

template <typename TScalar, typename TVector>
class SurfaceBase
{
public:
    using SurfaceBaseType = SurfaceBase<TScalar, TVector>;

    using ScalarType = TScalar;
    using VectorType = TVector;
    using IntervalType = Interval<ScalarType>;

public:
    static constexpr int
    Dimension()
    {
        return Internals::Dimension<VectorType>::value;
    }

    virtual IntervalType
    DomainU() const = 0;

    virtual IntervalType
    DomainV() const = 0;

    virtual VectorType
    PointAt(
        const ScalarType u,
        const ScalarType v) const = 0;

    virtual std::vector<VectorType>
    DerivativesAt(
        const ScalarType u,
        const ScalarType v,
        const int order) const = 0;

    virtual std::vector<IntervalType>
    SpansU() = 0;

    virtual std::vector<IntervalType>
    SpansV() = 0;
};

using SurfaceBase1D = SurfaceBase<double, Point1D>;
using SurfaceBase2D = SurfaceBase<double, Point2D>;
using SurfaceBase3D = SurfaceBase<double, Point3D>;

} // namespace ANurbs
