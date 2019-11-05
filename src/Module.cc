#include <anurbs/anurbs.h>

#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/stl_bind.h>

PYBIND11_MODULE(anurbs, m) {
    m.doc() = "A simple NURBS library for Python";
    m.attr("__author__") = "Thomas Oberbichler";
    m.attr("__copyright__") = "Copyright (c) 2018-2019, Thomas Oberbichler";
    m.attr("__version__") = ANURBS_VERSION;
    m.attr("__email__") = "thomas.oberbichler@gmail.com";
    m.attr("__status__") = "Development";

    using namespace anurbs;


    // --- Model

    auto model = Model::register_python(m);

    { // Attributes
        Attributes::register_python(m);
    }


    // --- Algorithm

    { // BrepEdgeIntegrationPoints
        BrepEdgeIntegrationPoints::register_python(m);
    }

    { // BrepFaceIntegrationPoints
        BrepFaceIntegrationPoints::register_python(m);
    }

    { // CurveSpanIntersection
        CurveSpanIntersection::register_python(m);
    }

    { // CurveTessellation
        CurveTessellation<2>::register_python(m);
        CurveTessellation<3>::register_python(m);
    }

    { // HilbertCurve
        HilbertCurve<2>::register_python(m);
        HilbertCurve<3>::register_python(m);
    }

    { // KnotRefinement
        KnotRefinement<2>::register_python(m);
        KnotRefinement<3>::register_python(m);
    }

    { // IntegrationPoints
        IntegrationPoints::register_python(m);
    }

    { // Nurbs
        Nurbs::register_python(m);
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

    { // PolygonTessellation
        PolygonTessellation::register_python(m);
    }

    { // RTree
        RTree<2>::register_python(m);
        RTree<3>::register_python(m);
    }

    { // TrimmedSurfaceClipping
        TrimmedSurfaceClipping::register_python(m);
    }


    // --- Geometry (base types)

    { // CurveBase
        CurveBase<2>::register_python(m);
        CurveBase<3>::register_python(m);
    }

    { // SurfaceBase
        SurfaceBase<2>::register_python(m);
        SurfaceBase<3>::register_python(m);
    }


    // --- Geometry

    { // Box
        Box<2>::register_python(m, model);
        Box<3>::register_python(m, model);
    }

    { // Brep
        Brep::register_python(m, model);
    }

    { // BrepEdge
        BrepEdge::register_python(m, model);
    }

    { // BrepFace
        BrepFace::register_python(m, model);
    }

    { // BrepFaceField
        BrepFaceField<1>::register_python(m, model);
        BrepFaceField<2>::register_python(m, model);
        BrepFaceField<3>::register_python(m, model);
        BrepFaceField<4>::register_python(m, model);
        BrepFaceField<5>::register_python(m, model);
    }

    { // BrepLoop
        BrepLoop::register_python(m, model);
    }

    { // BrepTrim
        BrepTrim::register_python(m, model);
    }

    { // Curve
        Curve<2, Ref<NurbsCurveGeometry<2>>>::register_python(m, model);
        Curve<3, Ref<NurbsCurveGeometry<3>>>::register_python(m, model);
    }

    { // CurveOnSurface
        CurveOnSurface<3>::register_python(m);
    }

    { // Interval
        Interval::register_python(m, model);
    }

    { // Line
        Line<2>::register_python(m, model);
        Line<3>::register_python(m, model);
    }

    { // NurbsCurveGeometry
        NurbsCurveGeometry<2>::register_python(m, model);
        NurbsCurveGeometry<3>::register_python(m, model);
    }

    { // NurbsSurfaceGeometry
        NurbsSurfaceGeometry<2>::register_python(m, model);
        NurbsSurfaceGeometry<3>::register_python(m, model);
    }

    { // Point
        Point<2>::register_python(m, model);
        Point<3>::register_python(m, model);
    }

    { // Polygon
        Polygon::register_python(m);
    }

    { // Polyline
        Polyline<2>::register_python(m, model);
        Polyline<3>::register_python(m, model);
    }

    { // Surface
        Surface<2, Ref<NurbsSurfaceGeometry<2>>>::register_python(m, model);
        Surface<3, Ref<NurbsSurfaceGeometry<3>>>::register_python(m, model);
    }

    m.def("test", [](Pointer<Line<3>> line) {return line;});
}