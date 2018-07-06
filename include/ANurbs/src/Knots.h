#pragma once

#include <algorithm>


namespace ANurbs {

class Knots
{
public:
    template <typename TScalar, typename TKnots>
    static std::size_t
    UpperSpan(
        const std::size_t& degree,
        const TKnots& knots,
        const TScalar& t
    )
    {
        size_t span = std::upper_bound(std::begin(knots) + degree,
            std::end(knots) - degree, t) - std::begin(knots) - 1;
        return span;
    }

    template <typename TScalar, typename TKnots>
    static std::size_t
    LowerSpan(
        const std::size_t& degree,
        const TKnots& knots,
        const TScalar& t
    )
    {
        size_t span = std::lower_bound(std::begin(knots) + degree,
            std::end(knots) - degree, t) - std::begin(knots) - 1;
        return span;
    }
};

} // namespace ANurbs
