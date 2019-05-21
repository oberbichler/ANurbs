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

Ref<Brep> BrepTrim::brep()
{
    return m_loop->face()->Brep();
}

Pointer<Curve<2>> BrepTrim::curve_2d()
{
    return new_<Curve<2>>(curve_geometry().Data(), domain());
}

Pointer<CurveOnSurface<3>> BrepTrim::curve_3d()
{
    return new_<CurveOnSurface<3>>(curve_geometry().Data(),
        face()->Geometry().Data(), domain());
}

Ref<NurbsCurveGeometry<2>> BrepTrim::curve_geometry()
{
    return m_curve_geometry;
}

Interval BrepTrim::domain()
{
    return m_domain;
}

Ref<BrepEdge> BrepTrim::edge()
{
    return m_edge;
}

Ref<BrepFace> BrepTrim::face()
{
    return m_loop->face();
}

Ref<BrepLoop> BrepTrim::loop()
{
    return m_loop;
}

} // namespace ANurbs