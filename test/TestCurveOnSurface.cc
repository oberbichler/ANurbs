#include "catch.hpp"

#include <ANurbs/ANurbs.h>

TEST_CASE( "Spatial B-Spline curve on surface", "[CurveOnSurface3d]" ) {
    using namespace ANurbs;
    
    using CurveGeometry2D = NurbsCurveGeometry<2>;

    Pointer<CurveGeometry2D> curveGeometry;
    {
        int degree = 2;
        int nbPoles = 3;
        bool isRational = false;

        curveGeometry = New<CurveGeometry2D>(degree, nbPoles, isRational);

        curveGeometry->set_knot( 0, 7.0);
        curveGeometry->set_knot( 1, 7.0);
        curveGeometry->set_knot( 2, 9.0);
        curveGeometry->set_knot( 3, 9.0);

        curveGeometry->set_pole(0, {3, 2});
        curveGeometry->set_pole(1, {1, 4});
        curveGeometry->set_pole(2, {2, 5});
    }

    Pointer<SurfaceGeometry3D> surfaceGeometry;
    {
        int degreeU = 2;
        int degreeV = 2;

        int nbPolesU = 3;
        int nbPolesV = 3;

        bool isRational = false;

        auto knotsU = {1, 1, 3, 3};

        surfaceGeometry = New<SurfaceGeometry3D>(degreeU, degreeV, nbPolesU,
            nbPolesV, isRational);

        surfaceGeometry->set_knotU(0, 1);
        surfaceGeometry->set_knotU(1, 1);
        surfaceGeometry->set_knotU(2, 3);
        surfaceGeometry->set_knotU(3, 3);

        surfaceGeometry->set_knotV(0, 2);
        surfaceGeometry->set_knotV(1, 2);
        surfaceGeometry->set_knotV(2, 6);
        surfaceGeometry->set_knotV(3, 6);

        surfaceGeometry->set_pole(0, 0, { 0,  0,  3});
        surfaceGeometry->set_pole(0, 1, { 0,  5,  0});
        surfaceGeometry->set_pole(0, 2, { 0, 10,  2});
        surfaceGeometry->set_pole(1, 0, { 5,  0,  5});
        surfaceGeometry->set_pole(1, 1, { 5,  5,  0});
        surfaceGeometry->set_pole(1, 2, { 5, 10,  3});
        surfaceGeometry->set_pole(2, 0, {10,  0,  1});
        surfaceGeometry->set_pole(2, 1, {10,  5, -1});
        surfaceGeometry->set_pole(2, 2, {10, 10,  0});
    }

    CurveOnSurface3D curveOnSurface(curveGeometry, surfaceGeometry,
        curveGeometry->Domain());

    SECTION( "Check point evaluation" ) {
        auto point = curveOnSurface.PointAt(8);

        REQUIRE( point[0] == Approx(3.75        ) );
        REQUIRE( point[1] == Approx(4.375       ) );
        REQUIRE( point[2] == Approx(1.5063476563) );
    }

    SECTION( "Check derivative evaluation" ) {
        auto point = curveOnSurface.DerivativesAt(8, 2);

        REQUIRE( point[0][0] == Approx( 3.75        ) );
        REQUIRE( point[0][1] == Approx( 4.375       ) );
        REQUIRE( point[0][2] == Approx( 1.5063476563) );

        REQUIRE( point[1][0] == Approx(-2.5         ) );
        REQUIRE( point[1][1] == Approx( 3.75        ) );
        REQUIRE( point[1][2] == Approx(-0.658203125 ) );
    }

    SECTION( "Check spans" ) {
        auto spans = curveOnSurface.Spans();

        REQUIRE( spans.size() == 1 );

        REQUIRE( spans[0].t0() == Approx(7.0) );
        REQUIRE( spans[0].t1() == Approx(9.0) );
    }
}
