import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal
from math import sqrt, cos, sin, tan, acos, asin, atan, pi

class TestCurveTessellation(unittest.TestCase):
    def test_tessellation_2d(self):
        geometry = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=4,
            is_rational=False,
        )

        geometry.set_knot(0, 0.0)
        geometry.set_knot(1, 0.0)
        geometry.set_knot(2, 1.0)
        geometry.set_knot(3, 2.0)
        geometry.set_knot(4, 2.0)

        geometry.set_pole(0, [0.0,  0.0])
        geometry.set_pole(1, [1.0,  1.0])
        geometry.set_pole(2, [2.0, -1.0])
        geometry.set_pole(3, [3.0, -1.0])

        tessellation = an.tessellate(curve=geometry, tolerance=1e-2)

        self.assertEqual(len(tessellation), 17)

        points = [point for _, point in tessellation]

        assert_array_almost_equal(
            points,
            [[0.0, 0.0],
             [0.2421875, 0.21875],
             [0.46875, 0.375],
             [0.6796875, 0.46875],
             [0.875, 0.5],
             [1.0546875, 0.46875],
             [1.21875, 0.375],
             [1.3671875, 0.21875],
             [1.5, 0.0],
             [1.6328125, -0.234375],
             [1.78125, -0.4375],
             [1.9453125, -0.609375],
             [2.125, -0.75],
             [2.3203125, -0.859375],
             [2.53125, -0.9375],
             [2.7578125, -0.984375],
             [3.0, -1.0]]
        )


if __name__ == '__main__':
    unittest.main()
