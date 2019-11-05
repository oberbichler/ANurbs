import unittest
import anurbs as an
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestNurbsCurveGeometry(unittest.TestCase):
    def test_curve_on_surface_3d(self):
        curve = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=3,
            is_rational=False,
        )

        curve.set_knot(0, 7)
        curve.set_knot(1, 7)
        curve.set_knot(2, 9)
        curve.set_knot(3, 9)

        curve.set_pole(0, [3, 2])
        curve.set_pole(1, [1, 4])
        curve.set_pole(2, [2, 5])

        surface = an.NurbsSurfaceGeometry3D(
            degree_u=2,
            degree_v=2,
            nb_poles_u=3,
            nb_poles_v=3,
            is_rational=False,
        )

        surface.set_knot_u(0, 1)
        surface.set_knot_u(1, 1)
        surface.set_knot_u(2, 3)
        surface.set_knot_u(3, 3)

        surface.set_knot_v(0, 2)
        surface.set_knot_v(1, 2)
        surface.set_knot_v(2, 6)
        surface.set_knot_v(3, 6)

        surface.set_pole(0, 0, [ 0,  0,  3])
        surface.set_pole(0, 1, [ 0,  5,  0])
        surface.set_pole(0, 2, [ 0, 10,  2])
        surface.set_pole(1, 0, [ 5,  0,  5])
        surface.set_pole(1, 1, [ 5,  5,  0])
        surface.set_pole(1, 2, [ 5, 10,  3])
        surface.set_pole(2, 0, [10,  0,  1])
        surface.set_pole(2, 1, [10,  5, -1])
        surface.set_pole(2, 2, [10, 10,  0])

        curve_on_surface = an.CurveOnSurface3D(curve, surface, curve.domain)

        assert_almost_equal(
            curve_on_surface.point_at(t=8),
            [3.75, 4.375, 1.5063476563]
        )

        assert_almost_equal(
            curve_on_surface.derivatives_at(t=8, order=2),
            [[3.75, 4.375, 1.5063476563],
             [-2.5, 3.75, -0.658203125],
             [7.5, -1.25, 1.1621094]],
        )

        spans = curve_on_surface.spans

        self.assertEqual(len(spans), 1)

        self.assertEqual(spans[0].t0, 7)
        self.assertEqual(spans[0].t1, 9)


if __name__ == '__main__':
    unittest.main()
