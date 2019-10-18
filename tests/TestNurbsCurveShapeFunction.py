import unittest
import ANurbs as an
from numpy.testing import assert_array_almost_equal

class TestNurbsCurveShapeFunction(unittest.TestCase):
    @staticmethod
    def test_without_weights():
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree=2,
            order=2,
            knots=[1, 1, 3, 3],
            t=2,
        )

        assert_array_almost_equal(
            nonzero_pole_indices,
            [0, 1, 2],
        )

        assert_array_almost_equal(
            shape_functions,
            [[ 0.25,  0.50, 0.25],
             [-0.50,  0.00, 0.50],
             [ 0.50, -1.00, 0.50]]
        )

    @staticmethod
    def test_with_equal_weights():
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree=2,
            order=2,
            knots=[1, 1, 3, 3],
            weights=[2, 2, 2],
            t=2,
        )

        assert_array_almost_equal(
            nonzero_pole_indices,
            [0, 1, 2],
        )

        assert_array_almost_equal(
            shape_functions,
            [[ 0.25,  0.50, 0.25],
             [-0.50,  0.00, 0.50],
             [ 0.50, -1.00, 0.50]]
        )

    @staticmethod
    def test_with_weights():
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree=2,
            order=2,
            knots=[1, 1, 3, 3],
            weights=[1, 1.5, 1],
            t=2,
        )

        assert_array_almost_equal(
            nonzero_pole_indices,
            [0, 1, 2],
        )

        assert_array_almost_equal(
            shape_functions,
            [[ 0.20,  0.60,  0.20],
             [-0.40,  0.00,  0.40],
             [ 0.48, -0.96,  0.48]]
        )

    @staticmethod
    def test_line_2d():
        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree=1,
            order=1,
            knots=[0, 5],
            t=1.0566243270259357,
        )

        assert_array_almost_equal(
            nonzero_pole_indices,
            [0, 1],
        )

        assert_array_almost_equal(
            shape_functions,
            [[ 0.788675134594813, 0.21132486540518713],
             [-0.200000000000000, 0.20000000000000000]],
        )

        nonzero_pole_indices, shape_functions = an.shape_functions(
            degree=1,
            order=1,
            knots=[0, 5],
            t=3.9433756729740645,
        )

        assert_array_almost_equal(
            nonzero_pole_indices,
            [0, 1],
        )

        assert_array_almost_equal(
            shape_functions,
            [[ 0.2113248654051871, 0.788675134594813],
             [-0.2000000000000000, 0.200000000000000]],
        )


if __name__ == '__main__':
    unittest.main()
