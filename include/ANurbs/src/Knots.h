#pragma once

#include <algorithm>

namespace ANurbs {

class Knots
{
public:
    template <typename TScalar, typename TKnots>
    static int
    UpperSpan(
        const int degree,
        const TKnots& knots,
        const TScalar& t)
    {
        size_t span = std::upper_bound(std::begin(knots) + degree,
            std::end(knots) - degree, t) - std::begin(knots) - 1;
        return static_cast<int>(span);
    }

    template <typename TScalar, typename TKnots>
    static int
    LowerSpan(
        const int degree,
        const TKnots& knots,
        const TScalar& t)
    {
        size_t span = std::lower_bound(std::begin(knots) + degree,
            std::end(knots) - degree, t) - std::begin(knots) - 1;
        return static_cast<int>(span);
    }
};

} // namespace ANurbs
