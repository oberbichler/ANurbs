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

        curve.knots = [7, 7, 9, 9]

        curve.poles = [
            [3, 2],
            [1, 4],
            [2, 5],
        ]

        surface = an.NurbsSurfaceGeometry3D(
            degree_u=2,
            degree_v=2,
            nb_poles_u=3,
            nb_poles_v=3,
            is_rational=False,
        )

        surface.knots_u = [1, 1, 3, 3]
        surface.knots_v = [2, 2, 6, 6]

        surface.poles = [
            [ 0,  0,  3],
            [ 0,  5,  0],
            [ 0, 10,  2],
            [ 5,  0,  5],
            [ 5,  5,  0],
            [ 5, 10,  3],
            [10,  0,  1],
            [10,  5, -1],
            [10, 10,  0],
        ]

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
