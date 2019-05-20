#include "catch.hpp"

#include <ANurbs/Geometry/Curve.h>
#include <ANurbs/Geometry/NurbsCurveGeometry.h>

using namespace ANurbs;


TEST_CASE( "Spatial B-Spline curve", "[Curve]" ) {
    int degree = 4;
    int nbPoles = 8;
    bool isRational = false;

    using Curve3D = Curve<3>;
    using CurveGeometry3D = NurbsCurveGeometry<3>;

    Pointer<CurveGeometry3D> geometry = New<CurveGeometry3D>(degree, nbPoles,
        isRational);

    geometry->set_knot( 0,  3.0);
    geometry->set_knot( 1,  3.0);
    geometry->set_knot( 2,  3.0);
    geometry->set_knot( 3,  3.0);
    geometry->set_knot( 4,  6.5);
    geometry->set_knot( 5, 10.0);
    geometry->set_knot( 6, 13.5);
    geometry->set_knot( 7, 17.0);
    geometry->set_knot( 8, 17.0);
    geometry->set_knot( 9, 17.0);
    geometry->set_knot(10, 17.0);

    geometry->set_pole(0, {  0, -25, - 5});
    geometry->set_pole(1, {-15, -15,   0});
    geometry->set_pole(2, {  5, - 5, - 3});
    geometry->set_pole(3, { 15, -15,   3});
    geometry->set_pole(4, { 25,   0,   6});
    geometry->set_pole(5, { 15,  15,   6});
    geometry->set_pole(6, {- 5, - 5, - 3});
    geometry->set_pole(7, {-25,  15,   4});

    // Curve3D curve(geometry, geometry->domain());

    // SECTION( "Compute arc length" ) {
    //     double length = curve.Length();

    //     CHECK( length == Approx(104.186832011) );
    // }

    // SECTION( "Compute arc length of segment" ) {
    //     double t0 =  5.0;
    //     double t1 = 10.0;

    //     double length = curve.SegmentLength(t0, t1);

    //     CHECK( length == Approx(24.4282553458) );
    // }
}
