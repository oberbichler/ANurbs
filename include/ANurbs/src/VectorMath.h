#pragma once

#include "Point.h"

#include <type_traits>

namespace ANurbs {
namespace Internals {

template <typename T>
struct Dimension;

template <typename TScalar, int TDimension>
struct Dimension<Point<TScalar, TDimension>>
{
    static int constexpr value = Point<TScalar, TDimension>::Dimension();
};

template <typename T>
struct Scalar;

template <typename TScalar, int TDimension>
struct Scalar<Point<TScalar, TDimension>>
{
    using type = typename Point<TScalar, TDimension>::ScalarType;
};

template <typename T>
struct Zero;

template <typename TScalar, int TDimension>
struct Zero<Point<TScalar, TDimension>>
{
    static Point<TScalar, TDimension> get() {
        return Point<TScalar, TDimension>();
    }
};

} // namespace Internals

template <typename TVector>
struct VectorMath
{
    using VectorType = TVector;
    using ScalarType = typename Internals::Scalar<VectorType>::type;

    static constexpr int
    Dimension()
    {
        return Internals::Dimension<VectorType>::value;
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

    template <int T = Dimension(),
        typename = typename std::enable_if<T == 2>::type>
    static ScalarType
    Cross(
        VectorType& u,
        VectorType& v
    )
    {
        return v[0] * u[1] - v[1] * u[0];
    }

    template <int T = Dimension(),
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
