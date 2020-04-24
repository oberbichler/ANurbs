import unittest
import anurbs as an
from numpy.testing import assert_array_almost_equal

class TestNurbsSurfaceGeometry(unittest.TestCase):
    @staticmethod
    def test_bspline_3d():
        surface_geometry = an.NurbsSurfaceGeometry3D(
            degree_u=2,
            degree_v=1,
            nb_poles_u=4,
            nb_poles_v=3,
            is_rational=False,
        )

        surface_geometry.knots_u = [0, 0, 7.5, 15, 15]
        surface_geometry.knots_v = [0, 10, 20]

        surface_geometry.poles = [
            [-10.0, - 5.0, -1.0         ],
            [-12.0,   3.0,  3.0         ],
            [- 9.0,  11.0, -0.0701928417],
            [- 5.0, - 3.0,  1.0         ],
            [- 6.0,   4.0, -2.0         ],
            [- 5.0,   7.0,  0.9298071583],
            [  0.0, - 4.0, -1.0         ],
            [  1.0,   6.0,  5.0         ],
            [  0.0,  13.0, -0.2350184214],
            [  4.0, - 2.0,  0.0         ],
            [  5.0,   4.0, -1.0         ],
            [  5.0,  11.0,  0.7649815786],
        ]

        model = an.Model()
        surface_geometry_ptr = model.add(surface_geometry)

        surface = an.Surface3D(surface_geometry_ptr)

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
