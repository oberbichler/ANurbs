import unittest
import anurbs as an
from numpy.testing import assert_array_almost_equal

class TestIntegrationPoints(unittest.TestCase):
    def test_throw_exception_for_unsupported_degree(self):
        def degree_too_low():
            an.integration_points(degree=0, domain=an.Interval(0, 1))

        self.assertRaises(Exception, degree_too_low)

        def degree_too_high():
            an.integration_points(degree=100, domain=an.Interval(0, 1))

        self.assertRaises(Exception, degree_too_high)

    @staticmethod
    def test_integration_points_0_to_1():
        integration_points = an.integration_points(
            degree=2,
            domain=an.Interval(0, 1),
        )

        assert_array_almost_equal(
            integration_points,
            [(0.21132486540518713, 0.5), (0.7886751345948129, 0.5)],
        )

    @staticmethod
    def test_integration_points_minus_1_to_1():
        integration_points = an.integration_points(
            degree=2,
            domain=an.Interval(-1, 1),
        )

        assert_array_almost_equal(
            integration_points,
            [(-0.5773502691896257, 1), (0.5773502691896257, 1)],
        )

    @staticmethod
    def test_integration_points_0_to_1_x_0_to_1():
        integration_points = an.integration_points(
            degree_u=2,
            degree_v=3,
            domain_u=an.Interval(0, 1),
            domain_v=an.Interval(0, 1),
        )

        assert_array_almost_equal(
            integration_points,
            [(0.21132486540518713, 0.1127016653792583, 0.13888888888888892),
             (0.21132486540518713, 0.5000000000000000, 0.22222222222222220),
             (0.21132486540518713, 0.8872983346207417, 0.13888888888888892),
             (0.78867513459481290, 0.1127016653792583, 0.13888888888888892),
             (0.78867513459481290, 0.5000000000000000, 0.22222222222222220),
             (0.78867513459481290, 0.8872983346207417, 0.13888888888888892)],
        )

    @staticmethod
    def test_integration_points_minus_1_to_1_x_minus_1_to_1():
        integration_points = an.integration_points(
            degree_u=2,
            degree_v=3,
            domain_u=an.Interval(-1, 1),
            domain_v=an.Interval(-1, 1),
        )

        assert_array_almost_equal(
            integration_points,
            [(-0.5773502691896257, -0.7745966692414834, 0.5555555555555557),
             (-0.5773502691896257,  0.0000000000000000, 0.8888888888888888),
             (-0.5773502691896257,  0.7745966692414834, 0.5555555555555557),
             ( 0.5773502691896257, -0.7745966692414834, 0.5555555555555557),
             ( 0.5773502691896257,  0.0000000000000000, 0.8888888888888888),
             ( 0.5773502691896257,  0.7745966692414834, 0.5555555555555557)],
        )

    @staticmethod
    def test_integration_points_curve_2d():
        model = an.Model()

        model.add_array(r"""[
            {
                "key": "TestData",
                "type": "nurbs_curve_geometry_2d",
                "degree": 1,
                "nb_poles": 2,
                "knots": [-1, 0, 1],
                "poles": [[-1, 0], [0, 0], [2, 0]]
            }
        ]""")

        curve = model.get(0).data

        integration_points = an.integration_points(
            degree=2,
            curve=curve,
        )

        assert_array_almost_equal(
            integration_points,
            [(-0.788675, 0.5),
             (-0.211325, 0.5),
             ( 0.211325, 1.0),
             ( 0.788675, 1.0)],
        )


    @staticmethod
    def test_integration_points_surface_3d():
        model = an.Model()

        model.add_array(r"""[
            {
                "type": "nurbs_surface_geometry_3d",
                "degree_u": 2,
                "degree_v": 1,
                "knots_u": [0.0,0.0,5.0,10.0,10.0],
                "knots_v": [0.0,20.0],
                "nb_poles_u": 4,
                "nb_poles_v": 2,
                "poles": [[0.0,0.0,0.0],[0.0,10.0,0.0],[2.00900107273569,0.0,0.0],[4.0763840660860486,7.8742350860349752,0.0],[7.0668900221629363,0.0,0.0],[12.011764418152659,7.1771412301834934,0.0],[10.0,0.0,0.0],[15.0,10.0,0.0]]
            }
        ]""")

        surface = model.get(0).data

        integration_points = an.integration_points(
            degree=2,
            surface=surface,
        )

        assert_array_almost_equal(
            integration_points,
            [(1.0566243270259357, 4.226497308103743, 11.759664385301264),
             (1.0566243270259357, 15.773502691896258, 17.31405070545194),
             (3.9433756729740645, 4.226497308103743, 10.883683768349144),
             (3.9433756729740645, 15.773502691896258, 15.000698744981156),
             (6.0566243270259355, 4.226497308103743, 10.603123001336892),
             (6.0566243270259355, 15.773502691896258, 12.7153914474524),
             (8.943375672974064, 4.226497308103743, 11.952054522910226),
             (8.943375672974064, 15.773502691896258, 9.795549688797168)],
        )


if __name__ == '__main__':
    unittest.main()
