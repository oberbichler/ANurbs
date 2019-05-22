import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestIntegrationPoints(unittest.TestCase):
    def test_throw_exception_for_unsupported_degree(self):
        def degree_too_low():
            an.integration_points(degree=0, domain=an.Interval(0, 1))

        self.assertRaises(Exception, degree_too_low)

        def degree_too_high():
            an.integration_points(degree=100, domain=an.Interval(0, 1))

        self.assertRaises(Exception, degree_too_high)

    def test_integration_points_0_to_1(self):
        integration_points = an.integration_points(
            degree=2,
            domain=an.Interval(0, 1),
        )

        assert_array_almost_equal(
            integration_points,
            [(0.21132486540518713, 0.5), (0.7886751345948129, 0.5)],
        )

    def test_integration_points_minus_1_to_1(self):
        integration_points = an.integration_points(
            degree=2,
            domain=an.Interval(-1, 1),
        )

        assert_array_almost_equal(
            integration_points,
            [(-0.5773502691896257, 1), (0.5773502691896257, 1)],
        )

    def test_integration_points_0_to_1_x_0_to_1(self):
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

    def test_integration_points_minus_1_to_1_x_minus_1_to_1(self):
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


if __name__ == '__main__':
    unittest.main()
