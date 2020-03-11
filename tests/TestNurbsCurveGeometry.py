import unittest
import anurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestNurbsCurveGeometry(unittest.TestCase):
    def test_bspline_3d(self):
        geometry = an.NurbsCurveGeometry3D(
            degree=4,
            nb_poles=8,
            is_rational=False,
        )

        geometry.knots[:4] = 0
        geometry.knots[4] = 32.9731425998736
        geometry.knots[5] = 65.9462851997473
        geometry.knots[6] = 98.9194277996209
        geometry.knots[7:] = 131.892570399495

        geometry.poles[0] = [  0, -25, - 5]
        geometry.poles[1] = [-15, -15,   0]
        geometry.poles[2] = [  5, - 5, - 3]
        geometry.poles[3] = [ 15, -15,   3]
        geometry.poles[4] = [ 25,   0,   6]
        geometry.poles[5] = [ 15,  15,   6]
        geometry.poles[6] = [- 5, - 5, - 3]
        geometry.poles[7] = [-25,  15,   4]

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
            geometry.derivatives_at(t=0.0, order=6),
            [[0, -25, -5],
             [-1.81966277, 1.2131085134, 0.6065542567],
             [0.2759310497, -0.0551862099, -0.0717420729],
             [-0.0189682773, 0.0005578905, 0.005523116],
             [0.0006062836, 0.0000493487, -0.0001894989],
             [0, 0, 0],
             [0, 0, 0]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=65.9462851997, order=6),
            [[18.8888888889, -6.1111111111, 4.1666666667],
             [0.2021847522, 0.336974587, 0.1010923761],
             [- 0.0122636022, 0.0153295028, -0.0036790807],
             [-0.0005578905, -0.0006508723, 0.0000557891],
             [-0.0000479387, -0.0000860077, 0.0000236874],
             [0, 0, 0],
             [0, 0, 0]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=131.892570399495, order=6),
            [[-25, 15, 4],
             [-2.4262170267, 2.4262170267, 0.8491759593],
             [-0.1103724199, 0.3311172597, 0.1269282829],
             [-0.0044631241, 0.0251050729, 0.0092051934],
             [-0.0001212567, 0.0008417473, 0.0002994759],
             [0, 0, 0],
             [0, 0, 0]],
        )

    def test_nurbs_3d(self):
        geometry = an.NurbsCurveGeometry3D(
            degree=4,
            nb_poles=8,
            is_rational=True,
        )

        geometry.knots[:4] = 0
        geometry.knots[4] = 32.9731425998736
        geometry.knots[5] = 65.9462851997473
        geometry.knots[6] = 98.9194277996209
        geometry.knots[7:] = 131.892570399495

        geometry.poles[0] = [  0, -25, - 5]
        geometry.poles[1] = [-15, -15,   0]
        geometry.poles[2] = [  5, - 5, - 3]
        geometry.poles[3] = [ 15, -15,   3]
        geometry.poles[4] = [ 25,   0,   6]
        geometry.poles[5] = [ 15,  15,   6]
        geometry.poles[6] = [- 5, - 5, - 3]
        geometry.poles[7] = [-25,  15,   4]

        geometry.weights[0] = 1.0
        geometry.weights[1] = 3.0
        geometry.weights[2] = 1.0
        geometry.weights[3] = 2.5
        geometry.weights[4] = 1.0
        geometry.weights[5] = 0.5
        geometry.weights[6] = 1.0
        geometry.weights[7] = 2.0

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
            [17.372881, -10.084746, 3.661017],
        )

        assert_array_almost_equal(
            geometry.point_at(t=125),
            [-15.801248, 7.432826, 1.456648],
        )

        assert_array_almost_equal(
            geometry.point_at(t=131.892570399495),
            [-25, 15, 4],
        )

        # derivarives_at

        assert_array_almost_equal(
            geometry.derivatives_at(t=0.0, order=6),
            [[0, -25, -5],
             [-5.458988, 3.639326, 1.819663],
             [3.421545, -2.152262, -1.12028],
             [-3.084298, 1.953733, 1.014747],
             [3.729439, -2.361026, -1.226293],
             [-5.633743, 3.566706, 1.85254],
             [10.212845893,-6.4657236846,-3.3582768272]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=65.9462851997, order=6),
            [[17.372881, -10.084746, 3.661017],
             [0.157519, 0.214672, 0.065029],
             [-0.001173, 0.013599, -0.00044],
             [-0.000212, -0.000031, 0.000078],
             [-0.000207, -0.00011, -0.000018],
             [-0.000022, -0.000027, -3.2301e-6],
             [-3.7928e-6,-5.3292e-6,-5.4212e-7]]
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=125, order=6),
            [[-15.801248, 7.432826, 1.456648],
             [-1.44436, 0.927174, 0.287317],
             [0.026303, 0.065941, 0.031612],
             [0.00346, -0.006864, -0.003416],
             [-0.000652, -0.000457, -0.000144],
             [0.000014, 0.000252, 0.000112],
             [0.0000210352,-0.0000274544,-0.0000147235]]
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=131.892570399495, order=6),
            [[-25, 15, 4],
             [-1.213109, 1.213109, 0.424588],
             [0.036791, 0.018395, 0.009198],
             [0, -0.005858, -0.00265],
             [-0.000309, 0.000486, 0.000244],
             [0.000056, 0.000032, 8.9157e-6],
             [-3.8613e-6,-0.0000221079,-9.4272e-6]]
        )

    def test_nurbs_3d_by_lists(self):
        geometry = an.NurbsCurveGeometry3D(
            degree=4,
            knots=[0, 0, 0, 0, 32.9731425998736, 65.9462851997473,
                   98.9194277996209, 131.892570399495, 131.892570399495,
                   131.892570399495, 131.892570399495],
            poles=[[0, -25, -5], [-15, -15, 0], [5, -5, -3], [15, -15, 3],
                   [25, 0, 6], [15, 15, 6], [-5, -5, -3], [-25, 15, 4]],
            weights=[1, 3, 1, 2.5, 1, 0.5, 1, 2],
        )

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
            [17.372881, -10.084746, 3.661017],
        )

        assert_array_almost_equal(
            geometry.point_at(t=125),
            [-15.801248, 7.432826, 1.456648],
        )

        assert_array_almost_equal(
            geometry.point_at(t=131.892570399495),
            [-25, 15, 4],
        )

        # derivarives_at

        assert_array_almost_equal(
            geometry.derivatives_at(t=0.0, order=5),
            [[0, -25, -5],
             [-5.458988, 3.639326, 1.819663],
             [3.421545, -2.152262, -1.12028],
             [-3.084298, 1.953733, 1.014747],
             [3.729439, -2.361026, -1.226293],
             [-5.633743, 3.566706, 1.85254]],
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=65.9462851997, order=5),
            [[17.372881, -10.084746, 3.661017],
             [0.157519, 0.214672, 0.065029],
             [-0.001173, 0.013599, -0.00044],
             [-0.000212, -0.000031, 0.000078],
             [-0.000207, -0.00011, -0.000018],
             [-0.000022, -0.000027, -3.2301e-6]]
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=125, order=5),
            [[-15.801248, 7.432826, 1.456648],
             [-1.44436, 0.927174, 0.287317],
             [0.026303, 0.065941, 0.031612],
             [0.00346, -0.006864, -0.003416],
             [-0.000652, -0.000457, -0.000144],
             [0.000014, 0.000252, 0.000112]]
        )

        assert_array_almost_equal(
            geometry.derivatives_at(t=131.892570399495, order=5),
            [[-25, 15, 4],
             [-1.213109, 1.213109, 0.424588],
             [0.036791, 0.018395, 0.009198],
             [0, -0.005858, -0.00265],
             [-0.000309, 0.000486, 0.000244],
             [0.000056, 0.000032, 8.9157e-6]]
        )

    def test_nurbs_3d_by_lists_throws(self):
        # valid input
        an.NurbsCurveGeometry3D(
            degree=4,
            knots=np.empty(8),
            poles=np.empty((5, 3)),
            weights=np.empty(5),
        )

        def invalid_knot_vector():
            an.NurbsCurveGeometry3D(
                degree=4,
                knots=np.empty(5),
                poles=np.empty((5, 3)),
            )

        self.assertRaises(Exception, invalid_knot_vector)

        def invalid_weight_vector():
            an.NurbsCurveGeometry3D(
                degree=4,
                knots=np.empty(8),
                poles=np.empty((5, 3)),
                weights=np.empty(6),
            )

        self.assertRaises(Exception, invalid_weight_vector)


if __name__ == '__main__':
    unittest.main()
