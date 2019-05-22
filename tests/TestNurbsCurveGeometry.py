import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestNurbsCurveGeometry(unittest.TestCase):
    def test_bspline_3d(self):
        geometry = an.NurbsCurveGeometry3D(
            degree=4,
            nb_poles=8,
            is_rational=False,
        )

        geometry.set_knot( 0,   0              )
        geometry.set_knot( 1,   0              )
        geometry.set_knot( 2,   0              )
        geometry.set_knot( 3,   0              )
        geometry.set_knot( 4,  32.9731425998736)
        geometry.set_knot( 5,  65.9462851997473)
        geometry.set_knot( 6,  98.9194277996209)
        geometry.set_knot( 7, 131.892570399495 )
        geometry.set_knot( 8, 131.892570399495 )
        geometry.set_knot( 9, 131.892570399495 )
        geometry.set_knot(10, 131.892570399495 )

        geometry.set_pole(0, [  0, -25, - 5])
        geometry.set_pole(1, [-15, -15,   0])
        geometry.set_pole(2, [  5, - 5, - 3])
        geometry.set_pole(3, [ 15, -15,   3])
        geometry.set_pole(4, [ 25,   0,   6])
        geometry.set_pole(5, [ 15,  15,   6])
        geometry.set_pole(6, [- 5, - 5, - 3])
        geometry.set_pole(7, [-25,  15,   4])

        self.assertEqual(geometry.dimension, 3)
        self.assertEqual(geometry.is_rational, False)

        self.assertEqual(geometry.degree, 4)
        self.assertEqual(geometry.nb_knots, 11)
        self.assertEqual(geometry.nb_poles, 8)

        self.assertEqual(geometry.domain.t0,   0             )
        self.assertEqual(geometry.domain.t1, 131.892570399495)

        # point_at

        assert_array_almost_equal(
            geometry.point_at(t=0.0),
            [0, -25, -5],
        )

        assert_array_almost_equal(
            geometry.point_at(t=65.9462851997),
            [18.8888888889, -6.1111111111, 4.1666666667],
        )

        assert_array_almost_equal(
            geometry.point_at(t=131.892570399495),
            [-25, 15, 4],
        )

        # derivarives_at

        assert_array_almost_equal(
            geometry.derivatives_at(t=0.0, order=3),
            [[0, -25, -5],
             [-1.81966277, 1.2131085134, 0.6065542567],
             [0.2759310497, -0.0551862099, -0.0717420729],
             [-0.0189682773, 0.0005578905, 0.005523116]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=65.9462851997, order=3),
            [[18.8888888889, -6.1111111111, 4.1666666667],
             [0.2021847522, 0.336974587, 0.1010923761],
             [- 0.0122636022, 0.0153295028, -0.0036790807],
             [-0.0005578905, -0.0006508723, 0.0000557891]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=131.892570399495, order=3),
            [[-25, 15, 4],
             [-2.4262170267, 2.4262170267, 0.8491759593],
             [-0.1103724199, 0.3311172597, 0.1269282829],
             [-0.0044631241, 0.0251050729, 0.0092051934]],
        )

    def test_nurbs_3d(self):
        geometry = an.NurbsCurveGeometry3D(
            degree=4,
            nb_poles=8,
            is_rational=True,
        )

        geometry.set_knot( 0,   0              )
        geometry.set_knot( 1,   0              )
        geometry.set_knot( 2,   0              )
        geometry.set_knot( 3,   0              )
        geometry.set_knot( 4,  32.9731425998736)
        geometry.set_knot( 5,  65.9462851997473)
        geometry.set_knot( 6,  98.9194277996209)
        geometry.set_knot( 7, 131.892570399495 )
        geometry.set_knot( 8, 131.892570399495 )
        geometry.set_knot( 9, 131.892570399495 )
        geometry.set_knot(10, 131.892570399495 )

        geometry.set_pole(0, [  0, -25, - 5])
        geometry.set_pole(1, [-15, -15,   0])
        geometry.set_pole(2, [  5, - 5, - 3])
        geometry.set_pole(3, [ 15, -15,   3])
        geometry.set_pole(4, [ 25,   0,   6])
        geometry.set_pole(5, [ 15,  15,   6])
        geometry.set_pole(6, [- 5, - 5, - 3])
        geometry.set_pole(7, [-25,  15,   4])

        geometry.set_weight(0, 1.0)
        geometry.set_weight(1, 1.0)
        geometry.set_weight(2, 1.0)
        geometry.set_weight(3, 2.5)
        geometry.set_weight(4, 1.0)
        geometry.set_weight(5, 1.0)
        geometry.set_weight(6, 1.0)
        geometry.set_weight(7, 1.0)

        self.assertEqual(geometry.dimension, 3)
        self.assertEqual(geometry.is_rational, True)

        self.assertEqual(geometry.degree, 4)
        self.assertEqual(geometry.nb_knots, 11)
        self.assertEqual(geometry.nb_poles, 8)

        self.assertEqual(geometry.domain.t0,   0             )
        self.assertEqual(geometry.domain.t1, 131.892570399495)

        # point_at

        assert_array_almost_equal(
            geometry.point_at(t=0.0),
            [0, -25, -5],
        )

        assert_array_almost_equal(
            geometry.point_at(t=65.9462851997),
            [21.333333, -3.666667, 4.9],
        )

        assert_array_almost_equal(
            geometry.point_at(t=131.892570399495),
            [-25, 15, 4],
        )

        # derivarives_at

        assert_array_almost_equal(
            geometry.derivatives_at(t=0.0, order=3),
            [[0, -25, -5],
             [-1.81966277, 1.2131085134, 0.6065542567],
             [0.2759310497, -0.0551862099, -0.0717420729],
             [-0.0189682773, 0.0005578905, 0.005523116]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=65.9462851997, order=3),
            [[21.33333333, -3.66666667, 4.9],
             [0.20218475, 0.33697459, 0.10109238],
             [-0.0122636, 0.0153295, -0.00367908],
             [-5.57890509e-04, -6.50872261e-04, 5.57890509e-05]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=131.892570399495, order=3),
            [[-25, 15, 4],
             [-2.4262170267, 2.4262170267, 0.8491759593],
             [-0.1103724199, 0.3311172597, 0.1269282829],
             [-0.0044631241, 0.0251050729, 0.0092051934]],
        )


if __name__ == '__main__':
    unittest.main()
