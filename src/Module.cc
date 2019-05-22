#include <ANurbs/ANurbs.h>

#include <pybind11/eigen.h>
#include <pybind11/stl_bind.h>

PYBIND11_MODULE(ANurbs, m) {
    m.doc() = "A simple NURBS library for Python";
    m.attr("__author__") = "Thomas Oberbichler";
    m.attr("__copyright__") = "Copyright (c) 2018-2019, Thomas Oberbichler";
    m.attr("__version__") = ANURBS_VERSION;
    m.attr("__email__") = "thomas.oberbichler@gmail.com";
    m.attr("__status__") = "Development";

    using namespace ANurbs;


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
}