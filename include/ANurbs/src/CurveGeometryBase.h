#pragma once

#include "CurveShapeEvaluator.h"
#include "Interval.h"
#include "Knots.h"

#include <vector>
#include <stdexcept>


namespace ANurbs {

template <typename TScalar, typename TVector>
class CurveGeometryBase
{
public:
    using ScalarType = TScalar;
    using VectorType = TVector;
    using KnotsType = std::vector<ScalarType>;
    using IntervalType = Interval<ScalarType>;

protected:
    std::size_t m_degree;
    KnotsType m_knots;

public:
    CurveGeometryBase(
        const std::size_t& degree,
        const std::size_t& nbPoles
    )
    : m_degree(degree)
    , m_knots(nbPoles + degree - 1)
    {
    }

    static constexpr std::size_t
    Dimension(
    )
    {
        return VectorMath<VectorType>::Dimension();
    }

    std::size_t
    Degree(
    ) const
    {
        return m_degree;
    }

    std::size_t
    NbKnots(
    ) const
    {
        return std::size_t(m_knots.size());
    }

    ScalarType
    Knot(
        const std::size_t& index
    ) const
    {
        return m_knots[index];
    }

    void
    SetKnot(
        const std::size_t& index,
        const ScalarType& value
    )
    {
        m_knots[index] = value;
    }

    const KnotsType&
    Knots(
    ) const
    {
        return m_knots;
    }

    std::size_t
    NbPoles(
    ) const
    {
        return NbKnots() - Degree() + 1;;
    }

    virtual VectorType
    Pole(
        const std::size_t& index
    ) const = 0;

    virtual void
    SetPole(
        const std::size_t& index,
        const VectorType& value
    ) = 0;

    virtual bool
    IsRational(
    ) const = 0;

    virtual ScalarType
    Weight(
        const std::size_t& index
    ) const = 0;

    virtual void
    SetWeight(
        const std::size_t& index,
        const ScalarType& value
    ) = 0;

    IntervalType
    Domain()
    {
        ScalarType t0 = Knot(Degree() - 1);
        ScalarType t1 = Knot(NbKnots() - Degree());

        return IntervalType(t0, t1);
    }

    std::size_t
    SpanAt(
        const ScalarType& t
    ) const
    {
        return Knots::LowerSpan(Degree(), Knots(), t);
    }

    template <typename TValue>
    TValue
    EvaluateAt(
        std::function<TValue(std::size_t)> getValue,
        const ScalarType& t
    )
    {
        // compute shape functions

        CurveShapeEvaluator<ScalarType> shape(Degree(), 0);

        if (IsRational()) {
            shape.Compute(Knots(), [&](std::size_t i) -> ScalarType {
                return Weight(i);
            }, t);
        } else {
            shape.Compute(Knots(), t);
        }

        // compute point

        TValue value = getValue(0) * shape(0, 0);

        for (std::size_t i = 1; i < shape.NbNonzeroPoles(); i++) {
            std::size_t index = shape.FirstNonzeroPole() + i;

            value += getValue(index) * shape(0, i);
        }

        return value;
    }

    template <typename TValue>
    std::vector<TValue>
    EvaluateAt(
        std::function<TValue(std::size_t)> getValue,
        const ScalarType& t,
        const std::size_t& order
    ) const
    {
        // evaluate shape functions

        CurveShapeEvaluator<ScalarType> shape(Degree(), order);

        if (IsRational()) {
            shape.Compute(Knots(), [&](std::size_t i) -> ScalarType {
                return Weight(i);
            }, t);
        } else {
            shape.Compute(Knots(), t);
        }

        // calculate derivatives

        std::vector<TValue> derivatives(shape.NbShapes());

        for (std::size_t order = 0; order < shape.NbShapes(); order++) {
            for (std::size_t i = 0; i < shape.NbNonzeroPoles(); i++) {
                std::size_t index = shape.FirstNonzeroPole() + i;

                if (i == 0) {
                    derivatives[order] = getValue(index) * shape(order, i);
                } else {
                    derivatives[order] += getValue(index) * shape(order, i);
                }
            }
        }

        return derivatives;
    }

    VectorType
    PointAt(
        const ScalarType& t
    )
    {
        return EvaluateAt<VectorType>([&](std::size_t i) -> VectorType {
            return Pole(i);
        }, t);
    }

    std::vector<VectorType>
    DerivativesAt(
        const ScalarType& t,
        const std::size_t& order
    ) const
    {
        return EvaluateAt<VectorType>([&](std::size_t i) -> VectorType {
            return Pole(i);
        }, t, order);
    }

    std::vector<IntervalType>
    Spans(
    )
    {
        std::size_t firstSpan = Degree() - 1;
        std::size_t lastSpan = NbKnots() - Degree() - 1;

        std::size_t nbSpans = lastSpan - firstSpan + 1;

        std::vector<IntervalType> result(nbSpans);

        for (std::size_t i = 0; i < nbSpans; i++) {
            ScalarType t0 = Knot(firstSpan + i);
            ScalarType t1 = Knot(firstSpan + i + 1);

            result[i] = IntervalType(t0, t1);
        }

        return result;
    }
};

} // namespace ANurbs
