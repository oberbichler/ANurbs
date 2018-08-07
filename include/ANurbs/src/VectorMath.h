#pragma once

#include "Point.h"

#include <type_traits>

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

    static ScalarType
    Norm(
        const ScalarType& scalar)
    {
        return std::abs(scalar);
    }

    template <int T = TDimension,
        typename = typename std::enable_if<T == 2>::type>
    static ScalarType
    Cross(
        VectorType& u,
        VectorType& v
    )
    {
        return v[0] * u[1] - v[1] * u[0];
    }

    template <int T = TDimension,
        typename = typename std::enable_if<T == 3>::type>
    static VectorType
    Cross(
        VectorType& u,
        VectorType& v
    )
    {
        VectorType result;

        result[0] = v[1] * u[2] - v[2] * u[1];
        result[1] = v[2] * u[0] - v[0] * u[2];
        result[2] = v[0] * u[1] - v[1] * u[0];

        return result;
    }
};

} // namespace ANurbs
