import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestNurbsSurfaceShapeFunction(unittest.TestCase):
    def test_without_weights(self):
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree_u=2,
            degree_v=1,
            order=1,
            knots_u=[0, 0, 7.5, 15, 15],
            knots_v=[0, 10, 20],
            u=12,
            v=5,
        )

        assert_almost_equal(
            nonzero_pole_indices,
            [(1, 0), (1, 1), (2, 0), (2, 1), (3, 0), (3, 1)],
        )

        assert_array_almost_equal(
            shape_functions[:2],
            [[0.04, 0.04, 0.28, 0.28, 0.18, 0.18],
             [-0.0266666667, -0.0266666667, -0.0533333333, -0.0533333333, 0.08, 0.08]],
        )

    def test_with_equal_weights(self):
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree_u=2,
            degree_v=1,
            order=1,
            knots_u=[0, 0, 7.5, 15, 15],
            knots_v=[0, 10, 20],
            weights=[[2, 2, 2], [2, 2, 2], [2, 2, 2], [2, 2, 2]],
            u=12,
            v=5,
        )

        assert_almost_equal(
            nonzero_pole_indices,
            [(1, 0), (1, 1), (2, 0), (2, 1), (3, 0), (3, 1)],
        )

        assert_array_almost_equal(
            shape_functions[:2],
            [[0.04, 0.04, 0.28, 0.28, 0.18, 0.18],
             [-0.0266666667, -0.0266666667, -0.0533333333, -0.0533333333, 0.08, 0.08]],
        )

    def test_with_weights(self):
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree_u=2,
            degree_v=1,
            order=1,
            knots_u=[0, 0, 7.5, 15, 15],
            knots_v=[0, 10, 20],
            weights=[[1, 1, 1], [1, 2.5, 1], [1, 1, 1], [1, 1, 1]],
            u=12,
            v=5,
        )

        assert_almost_equal(
            nonzero_pole_indices,
            [(1, 0), (1, 1), (2, 0), (2, 1), (3, 0), (3, 1)],
        )

        assert_array_almost_equal(
            shape_functions[:2],
            [[ 0.0377358491,  0.0943396226,  0.2641509434,  0.2641509434, 0.1698113208, 0.1698113208],
             [-0.0237332384, -0.0593330960, -0.0403465053, -0.0403465053, 0.0818796725, 0.0818796725]],
        )


if __name__ == '__main__':
    unittest.main()
