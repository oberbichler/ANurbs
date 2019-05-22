import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestNurbsSurfaceGeometry(unittest.TestCase):
    def test_bspline_3d(self):
        surface_geometry = an.NurbsSurfaceGeometry3D(
            degree_u=2,
            degree_v=1,
            nb_poles_u=4,
            nb_poles_v=3,
            is_rational=False,
        )

        surface_geometry.set_knot_u(0,  0.0)
        surface_geometry.set_knot_u(1,  0.0)
        surface_geometry.set_knot_u(2,  7.5)
        surface_geometry.set_knot_u(3, 15.0)
        surface_geometry.set_knot_u(4, 15.0)

        surface_geometry.set_knot_v(0,  0.0)
        surface_geometry.set_knot_v(1, 10.0)
        surface_geometry.set_knot_v(2, 20.0)

        surface_geometry.set_pole(0, 0, [-10.0, - 5.0, -1.0         ])
        surface_geometry.set_pole(0, 1, [-12.0,   3.0,  3.0         ])
        surface_geometry.set_pole(0, 2, [- 9.0,  11.0, -0.0701928417])
        surface_geometry.set_pole(3,    [- 5.0, - 3.0,  1.0         ])
        surface_geometry.set_pole(1, 1, [- 6.0,   4.0, -2.0         ])
        surface_geometry.set_pole(1, 2, [- 5.0,   7.0,  0.9298071583])
        surface_geometry.set_pole(2, 0, [  0.0, - 4.0, -1.0         ])
        surface_geometry.set_pole(2, 1, [  1.0,   6.0,  5.0         ])
        surface_geometry.set_pole(2, 2, [  0.0,  13.0, -0.2350184214])
        surface_geometry.set_pole(3, 0, [  4.0, - 2.0,  0.0         ])
        surface_geometry.set_pole(3, 1, [  5.0,   4.0, -1.0         ])
        surface_geometry.set_pole(3, 2, [  5.0,  11.0,  0.7649815786])

        surface = an.Surface3D(surface_geometry)

        # point_at

        assert_array_almost_equal(
            surface.point_at(u=12, v=5),
            [1.46, 0.96, 0.9],
        )

        # derivarives_at

        assert_array_almost_equal(
            surface.derivatives_at(u=12, v=5, order=2),
            [[1.46, 0.96, 0.9],
             [0.96, 0.0266666667, -0.2666666667],
             [0.084, 0.832, 0.276],
             [0.0355555556, -0.0088888889, -0.1333333333],
             [0.0106666667, -0.048, -0.064],
             [0, 0, 0]],
        )


if __name__ == '__main__':
    unittest.main()
