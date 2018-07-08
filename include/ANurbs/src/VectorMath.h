#pragma once

#include "Point.h"

namespace ANurbs {

template <typename Point>
struct VectorMath;

template <typename TScalar, int TDimension>
struct VectorMath<Point<TScalar, TDimension>>
{
    using VectorType = Point<TScalar, TDimension>;
    using ScalarType = TScalar;

    static constexpr int
    Dimension()
    {
        return TDimension;
    }

    static VectorType
    Zero()
    {
        return VectorType();
    }

    static ScalarType
    Dot(
        const VectorType& u,
        const VectorType& v)
    {
        ScalarType result{ 0 };

        for (int i = 0; i < Dimension(); i++) {
            result += u[i] * v[i];
        }

        return result;
    }

    static ScalarType
    SquaredNorm(
        const VectorType& vector)
    {
        ScalarType sqSum{ 0 };

        for (int i = 0; i < Dimension(); i++) {
            sqSum += vector[i] * vector[i];
        }

        return sqSum;
    }

    static ScalarType
    Norm(
        const VectorType& vector)
    {
        ScalarType sqSum = SquaredNorm(vector);

        return std::sqrt(sqSum);
    }
};

} // namespace ANurbs
