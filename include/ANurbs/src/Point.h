#pragma once

#include <array>
#include <assert.h>


namespace ANurbs {

template <typename TScalar, std::size_t TDimension>
class Point
{
public:
    using PointType = Point<TScalar, TDimension>;
    using ScalarType = TScalar;

private:
    std::array<double, 3> m_coordinates;

public:
    Point(
    )
    : m_coordinates({})
    {
    }

    Point(
        const Point& other
    ) : m_coordinates(other.m_coordinates)
    {
    }

    Point(
        std::initializer_list<ScalarType> coordinates
    )
    {
        assert(coordinates.size() == TDimension && "Wrong dimension");
        
        auto it = coordinates.begin();
        for (std::size_t i = 0; i < Dimension(); i++) {
            m_coordinates[i] = *it++;
        }
    }

    static constexpr std::size_t
    Dimension()
    {
        return TDimension;
    }

    const ScalarType&
    operator[](
        const std::size_t& index
    ) const
    {
        return m_coordinates[index];
    }

    ScalarType&
    operator[](
        const std::size_t& index
    )
    {
        assert(index < Dimension() && "Index out of range");

        return m_coordinates[index];
    }

    ScalarType
    X(
    ) const
    {
        static_assert(Dimension() >= 1, "Index out of range");
        return m_coordinates[0];
    }

    ScalarType&
    X(
    )
    {
        static_assert(Dimension() >= 1, "Index out of range");
        return m_coordinates[0];
    }

    ScalarType
    Y(
    ) const
    {
        static_assert(Dimension() >= 2, "Index out of range");
        return m_coordinates[1];
    }

    ScalarType&
    Y(
    )
    {
        static_assert(Dimension() >= 2, "Index out of range");
        return m_coordinates[1];
    }

    ScalarType
    Z(
    ) const
    {
        static_assert(Dimension() >= 3, "Index out of range");
        return m_coordinates[2];
    }

    ScalarType&
    Z(
    )
    {
        static_assert(Dimension() >= 3, "Index out of range");
        return m_coordinates[2];
    }

    friend PointType
    operator+(
        PointType lhs,
        const PointType& rhs
    )
    {
        for (std::size_t i = 0; i < Dimension(); i++) {
            lhs[i] += rhs[i];
        }

        return lhs;
    }

    friend PointType
    operator+=(
        PointType& lhs,
        const PointType& rhs
    )
    {
        for (std::size_t i = 0; i < Dimension(); i++) {
            lhs[i] += rhs[i];
        }

        return lhs;
    }

    friend PointType
    operator-(
        PointType lhs,
        const PointType& rhs
    )
    {
        for (std::size_t i = 0; i < Dimension(); i++) {
            lhs[i] -= rhs[i];
        }

        return lhs;
    }

    friend PointType
    operator*(
        PointType lhs,
        const ScalarType& rhs
    )
    {
        for (std::size_t i = 0; i < Dimension(); i++) {
            lhs[i] *= rhs;
        }

        return lhs;
    }
};

using Point1D = Point<double, 1>;
using Point2D = Point<double, 2>;
using Point3D = Point<double, 3>;

} // namespace ANurbs
