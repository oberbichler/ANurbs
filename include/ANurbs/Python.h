#pragma once

#include "ANurbs.h"

#include "Model/Model.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <functional>
#include <string>
#include <unordered_map>

template <typename TModule>
void
RegisterData(
    TModule& m)
{
    using namespace ANurbs;
    using namespace pybind11::literals;
    namespace py = pybind11;


    // --- Model

    auto model = Model::register_python(m);


    // --- Algorithm

    { // CurveSpanIntersection
        CurveSpanIntersection::register_python(m);
    }

    { // CurveTessellation
        CurveTessellation<2>::register_python(m);
        CurveTessellation<3>::register_python(m);
    }

    { // KnotVector
        ANurbs::KnotVector::register_python(m);
    }

    { // KnotRefinement
        KnotRefinement<2>::register_python(m);
        KnotRefinement<3>::register_python(m);
    }

    { // IntegrationPoints
        IntegrationPoints::register_python(m);
    }

    { // NurbsCurveShapeFunction
        NurbsCurveShapeFunction::register_python(m);
    }

    { // NurbsSurfaceShapeFunction
        NurbsSurfaceShapeFunction::register_python(m);
    }

    { // PointOnCurveProjection
        PointOnCurveProjection<2>::register_python(m);
        PointOnCurveProjection<3>::register_python(m);
    }

    { // PointOnSurfaceProjection
        PointOnSurfaceProjection<3>::register_python(m);
    }

    { // PolygonIntegrationPoints
        PolygonIntegrationPoints::register_python(m);
    }

    { // Polyline
        Polyline<2>::register_python(m, model);
        Polyline<3>::register_python(m, model);
    }

    { // PolygonTessellation
        PolygonTessellation::register_python(m);
    }

    { // TrimmedSurfaceClipping
        TrimmedSurfaceClipping::register_python(m);
    }


    // --- Geometry (base types)

    { // CurveBase
        CurveBase<2>::register_python(m);
        CurveBase<3>::register_python(m);
    }
    
    { // NurbsCurveGeometry
        NurbsCurveGeometry<2>::register_python(m, model);
        NurbsCurveGeometry<3>::register_python(m, model);
    }


    // --- Geometry

    { // Brep
        Brep::register_python(m, model);
    }

    { // BrepEdge
        BrepEdge::register_python(m, model);
    }

    { // BrepFace
        BrepFace::register_python(m, model);
    }

    { // BrepLoop
        BrepLoop::register_python(m, model);
    }

    { // BrepTrim
        BrepTrim::register_python(m, model);
    }

    { // Curve
        Curve<2>::register_python(m, model);
        Curve<3>::register_python(m, model);
    }

    { // Interval
        Interval::register_python(m, model);
    }
    
    { // Line
        Line<2>::register_python(m, model);
        Line<3>::register_python(m, model);
    }
    
    { // SurfaceBase
        SurfaceBase<2>::register_python(m);
        SurfaceBase<3>::register_python(m);
    }
    
    { // NurbsSurfaceGeometry
        NurbsSurfaceGeometry<2>::register_python(m, model);
        NurbsSurfaceGeometry<3>::register_python(m, model);
    }
    
    { // Surface
        Surface<2>::register_python(m, model);
        Surface<3>::register_python(m, model);
    }

    { // CurveOnSurface
        CurveOnSurface<3>::register_python(m);
    }

    { // Polygon
        Polygon::register_python(m);
    }


    // --- Model

    { // Attributes
        Attributes::register_python(m);
    }

    { // CadAttributes
        CadAttributes::register_python(m);
    }
};