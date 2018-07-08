#pragma once

#include "Interval.h"
#include "Math.h"
#include "VectorMath.h"

#include <vector>

namespace ANurbs {

template <typename TScalar, typename TVector>
class CurveBase
{
public:
    using CurveBaseType = CurveBase<TScalar, TVector>;

    using ScalarType = TScalar;
    using VectorType = TVector;
    using IntervalType = Interval<ScalarType>;

public:
    static constexpr int
    Dimension()
    {
        return VectorMath<VectorType>::Dimension();
    }

    virtual IntervalType
    Domain() const = 0;

    virtual VectorType
    PointAt(
        const ScalarType& t) const = 0;

    virtual std::vector<VectorType>
    DerivativesAt(
        const ScalarType& t,
        const int& order) const = 0;

    ScalarType
    Length(
        const ScalarType& tolerance = 1e-7) const
    {
        ScalarType t0 = Domain().T0();
        ScalarType t1 = Domain().T1();

        return SegmentLength(t0, t1, tolerance);
    }

    ScalarType
    SegmentLength(
        const IntervalType& domain,
        const ScalarType& tolerance = 1e-7) const
    {
        return SegmentLength(domain.T0(), domain.T1(), tolerance);
    }

    ScalarType
    SegmentLength(
        const ScalarType& t0,
        const ScalarType& t1,
        const ScalarType& tolerance = 1e-7) const
    {
        auto f = [&](ScalarType t) -> ScalarType {
            auto tangent = DerivativesAt(t, 1)[1];
            return VectorMath<VectorType>::Norm(tangent);
        };

        return Math::Romberg(f, t0, t1, 1000, tolerance);
    }

    virtual std::vector<IntervalType>
    Spans() = 0;
};

} // namespace ANurbs
