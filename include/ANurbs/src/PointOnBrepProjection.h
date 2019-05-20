#pragma once

#include <utility>

namespace ANurbs {

template <typename TBrep>
class PointOnBrepProjection
{
public:

private:
    Pointer<TBrep> m_brep;
    double m_tolerance;

public:
    PointOnBrepProjection(
        Pointer<TBrep> brep,
        const double tolerance)
    : m_brep(brep)
    , m_tolerance(tolerance)
    {
    }
};

} // namespace ANurbs
