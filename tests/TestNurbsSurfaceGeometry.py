import unittest
import anurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestNurbsSurfaceGeometry(unittest.TestCase):
    def test_bspline_3d(self):
        surface = an.NurbsSurfaceGeometry3D(
            degree_u=2,
            degree_v=1,
            nb_poles_u=4,
            nb_poles_v=3,
            is_rational=False,
        )

        surface.set_knot_u(0,  0.0)
        surface.set_knot_u(1,  0.0)
        surface.set_knot_u(2,  7.5)
        surface.set_knot_u(3, 15.0)
        surface.set_knot_u(4, 15.0)

        surface.set_knot_v(0,  0.0)
        surface.set_knot_v(1, 10.0)
        surface.set_knot_v(2, 20.0)

        surface.set_pole(0, 0, [-10.0, - 5.0, -1.0         ])
        surface.set_pole(0, 1, [-12.0,   3.0,  3.0         ])
        surface.set_pole(0, 2, [- 9.0,  11.0, -0.0701928417])
        surface.set_pole(3,    [- 5.0, - 3.0,  1.0         ])
        surface.set_pole(1, 1, [- 6.0,   4.0, -2.0         ])
        surface.set_pole(1, 2, [- 5.0,   7.0,  0.9298071583])
        surface.set_pole(2, 0, [  0.0, - 4.0, -1.0         ])
        surface.set_pole(2, 1, [  1.0,   6.0,  5.0         ])
        surface.set_pole(2, 2, [  0.0,  13.0, -0.2350184214])
        surface.set_pole(3, 0, [  4.0, - 2.0,  0.0         ])
        surface.set_pole(3, 1, [  5.0,   4.0, -1.0         ])
        surface.set_pole(3, 2, [  5.0,  11.0,  0.7649815786])

        self.assertEqual(surface.dimension, 3)

        self.assertEqual(surface.degree_u, 2)
        self.assertEqual(surface.degree_v, 1)

        self.assertEqual(surface.nb_knots_u, 5)
        self.assertEqual(surface.nb_knots_v, 3)

        self.assertEqual(surface.nb_poles_u, 4)
        self.assertEqual(surface.nb_poles_v, 3)
        self.assertEqual(surface.nb_poles, 12)

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

        # normal_at

        assert_array_almost_equal(
            surface.normal_at(u=12, v=5),
            [0.2613127124, -0.3275832699, 0.9079674375],
        )

    def test_nurbs_3d(self):
        surface = an.NurbsSurfaceGeometry3D(
            degree_u=2,
            degree_v=1,
            nb_poles_u=4,
            nb_poles_v=3,
            is_rational=True,
        )

        surface.set_knot_u(0,  0.0)
        surface.set_knot_u(1,  0.0)
        surface.set_knot_u(2,  7.5)
        surface.set_knot_u(3, 15.0)
        surface.set_knot_u(4, 15.0)

        surface.set_knot_v(0,  0.0)
        surface.set_knot_v(1, 10.0)
        surface.set_knot_v(2, 20.0)

        surface.set_pole(0, 0, [-10.0, - 5.0, -1.0         ])
        surface.set_pole(0, 1, [-12.0,   3.0,  3.0         ])
        surface.set_pole(0, 2, [- 9.0,  11.0, -0.0701928417])
        surface.set_pole(3,    [- 5.0, - 3.0,  1.0         ])
        surface.set_pole(1, 1, [- 6.0,   4.0, -2.0         ])
        surface.set_pole(1, 2, [- 5.0,   7.0,  0.9298071583])
        surface.set_pole(2, 0, [  0.0, - 4.0, -1.0         ])
        surface.set_pole(2, 1, [  1.0,   6.0,  5.0         ])
        surface.set_pole(2, 2, [  0.0,  13.0, -0.2350184214])
        surface.set_pole(3, 0, [  4.0, - 2.0,  0.0         ])
        surface.set_pole(3, 1, [  5.0,   4.0, -1.0         ])
        surface.set_pole(3, 2, [  5.0,  11.0,  0.7649815786])

        surface.set_weight(0, 0, 1.0)
        surface.set_weight(0, 1, 1.0)
        surface.set_weight(0, 2, 1.0)
        surface.set_weight(1, 0, 1.0)
        surface.set_weight(1, 1, 2.5)
        surface.set_weight(1, 2, 1.0)
        surface.set_weight(2, 0, 1.0)
        surface.set_weight(7,    1.0)
        surface.set_weight(2, 2, 1.0)
        surface.set_weight(3, 0, 1.0)
        surface.set_weight(3, 1, 1.0)
        surface.set_weight(3, 2, 1.0)

        self.assertEqual(surface.dimension, 3)

        self.assertEqual(surface.degree_u, 2)
        self.assertEqual(surface.degree_v, 1)

        self.assertEqual(surface.nb_knots_u, 5)
        self.assertEqual(surface.nb_knots_v, 3)

        self.assertEqual(surface.nb_poles_u, 4)
        self.assertEqual(surface.nb_poles_v, 3)
        self.assertEqual(surface.nb_poles, 12)

        # point_at

        assert_array_almost_equal(
            surface.point_at(u=12, v=5),
            [1.0377358491, 1.1320754717, 0.7358490566],
        )

        # derivarives_at

        assert_array_almost_equal(
            surface.derivatives_at(u=12, v=5, order=2),
            [[ 1.0377358491,  1.1320754717,  0.7358490566],
             [ 1.1712353151, -0.0830663344, -0.1483327400],
             [-0.0004271983,  0.8173727305,  0.2294054824],
             [ 0.0334131158,  0.0214196208, -0.1713942830],
             [ 0.0499024922, -0.0351431047, -0.0293933919],
             [ 9.6724e-6   , -0.0185065524, -0.0051940864]],
        )

        # normal_at

        assert_array_almost_equal(
            surface.normal_at(u=12, v=5),
            [0.1022371052, -0.2687554542, 0.9577672369],
        )

if __name__ == '__main__':
    unittest.main()
