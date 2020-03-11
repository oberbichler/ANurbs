import unittest
import anurbs as an
from numpy.testing import assert_array_almost_equal


class TestCurve(unittest.TestCase):
    @staticmethod
    def test_bspline_3d():
        curve_geometry = an.NurbsCurveGeometry3D(
            degree=4,
            nb_poles=8,
            is_rational=False,
        )

        curve_geometry.knots[:4] = 0
        curve_geometry.knots[4] = 32.9731425998736
        curve_geometry.knots[5] = 65.9462851997473
        curve_geometry.knots[6] = 98.9194277996209
        curve_geometry.knots[7:] = 131.892570399495

        curve_geometry.poles[0] = [  0, -25, - 5]
        curve_geometry.poles[1] = [-15, -15,   0]
        curve_geometry.poles[2] = [  5, - 5, - 3]
        curve_geometry.poles[3] = [ 15, -15,   3]
        curve_geometry.poles[4] = [ 25,   0,   6]
        curve_geometry.poles[5] = [ 15,  15,   6]
        curve_geometry.poles[6] = [- 5, - 5, - 3]
        curve_geometry.poles[7] = [-25,  15,   4]

        model = an.Model()
        curve_geometry_ptr = model.add(curve_geometry)

        curve = an.Curve3D(curve_geometry_ptr, curve_geometry.domain)

        # point_at

        assert_array_almost_equal(
            curve.point_at(t=65.9462851997),
            [18.8888888889, -6.1111111111, 4.1666666667],
        )

        #SECTION( "Compute arc length" ) {
        #    double length = curve.Length();

        #    CHECK( length == Approx(104.186832011) );
        #}

        #SECTION( "Compute arc length of segment" ) {
        #    double t0 =  5.0;
        #    double t1 = 10.0;

        #    double length = curve.SegmentLength(t0, t1);

        #    CHECK( length == Approx(24.4282553458) );
        #}


if __name__ == '__main__':
    unittest.main()
