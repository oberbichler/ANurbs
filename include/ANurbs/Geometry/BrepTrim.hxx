#pragma once

#include "BrepTrim.h"

#include "BrepEdge.h"
#include "Curve.h"
#include "CurveOnSurface.h"
#include "NurbsCurveGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

Ref<Brep> BrepTrim::brep() const
{
    return m_loop->face()->brep();
}

Pointer<Curve<2>> BrepTrim::curve_2d() const
{
    return new_<Curve<2>>(curve_geometry().data(), domain());
}

Pointer<CurveOnSurface<3>> BrepTrim::curve_3d() const
{
    return new_<CurveOnSurface<3>>(curve_geometry().data(),
        face()->surface_geometry().data(), domain());
}

Ref<NurbsCurveGeometry<2>> BrepTrim::curve_geometry() const
{
    return m_curve_geometry;
}

Interval BrepTrim::domain() const
{
    return m_domain;
}

Ref<BrepEdge> BrepTrim::edge() const
{
    return m_edge;
}

Ref<BrepFace> BrepTrim::face() const
{
    return m_loop->face();
}

Ref<BrepLoop> BrepTrim::loop() const
{
    return m_loop;
}

Ref<NurbsSurfaceGeometry<3>> BrepTrim::surface_geometry() const
{
    return face()->surface_geometry();
}

} // namespace ANurbs