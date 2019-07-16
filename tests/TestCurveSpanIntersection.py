import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal

class TestCurveSpanIntersection(unittest.TestCase):
    def test_example_1(self):
        curve_geometry = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=5,
            is_rational=False,
        )

        curve_geometry.set_knot(0,  1.0         )
        curve_geometry.set_knot(1,  1.0         )
        curve_geometry.set_knot(2,  4.3333333333)
        curve_geometry.set_knot(3,  7.6666666667)
        curve_geometry.set_knot(4, 11.0         )
        curve_geometry.set_knot(5, 11.0         )

        curve_geometry.set_pole(0, [5.0,  5.0])
        curve_geometry.set_pole(1, [8.0,  7.0])
        curve_geometry.set_pole(2, [6.0,  8.0])
        curve_geometry.set_pole(3, [8.0, 10.0])
        curve_geometry.set_pole(4, [9.0,  8.0])

        intersections = an.curve_span_intersection(curve_geometry,
            knots_u=[5, 5, 6, 7, 8, 9, 10, 10],
            knots_v=[5, 5, 6, 8, 8, 10, 10],
            tolerance=1e-7,
            include_curve_knots=False,
        )

        assert_array_almost_equal(
            intersections,
            [1, 1.6366100188, 1.9308025998, 2.6666666667, 4.3333333333,
             5.7140452079, 7.6666666667, 9.3333333333, 11],
        )

    def test_example_2(self):
        curve_geometry = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=5,
            is_rational=False,
        )

        curve_geometry.set_knot(0, 2)
        curve_geometry.set_knot(1, 2)
        curve_geometry.set_knot(2, 3)
        curve_geometry.set_knot(3, 3)
        curve_geometry.set_knot(4, 4)
        curve_geometry.set_knot(5, 4)

        curve_geometry.set_pole(0, [ 8.0,  6.0])
        curve_geometry.set_pole(1, [ 6.0,  7.0])
        curve_geometry.set_pole(2, [ 8.0,  8.0])
        curve_geometry.set_pole(3, [10.0,  9.0])
        curve_geometry.set_pole(4, [ 8.0, 10.0])

        intersections = an.curve_span_intersection(curve_geometry,
            knots_u=[5, 5, 6, 7, 8, 9, 10, 10],
            knots_v=[5, 5, 6, 8, 8, 10, 10],
            tolerance=1e-7,
            include_curve_knots=False,
        )

        assert_array_almost_equal(
            intersections,
            [2, 2.5, 3, 3.5, 4],
        )

    def test_example_3(self):
        curve_geometry = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=4,
            is_rational=False,
        )

        curve_geometry.set_knot(0, 1)
        curve_geometry.set_knot(1, 1)
        curve_geometry.set_knot(2, 2)
        curve_geometry.set_knot(3, 3)
        curve_geometry.set_knot(4, 3)

        curve_geometry.set_pole(0, [9.0, 6.0])
        curve_geometry.set_pole(1, [9.0, 7.0])
        curve_geometry.set_pole(2, [6.0, 7.0])
        curve_geometry.set_pole(3, [6.0, 8.0])

        intersections = an.curve_span_intersection(curve_geometry,
            knots_u=[5, 5, 6, 7, 8, 9, 10, 10],
            knots_v=[5, 5, 6, 8, 8, 10, 10],
            tolerance=1e-7,
            include_curve_knots=False,
        )

        assert_array_almost_equal(
            intersections,
            [1, 1.8164965809, 2.1835034191, 3],
        )

    def test_example_4(self):
        curve_geometry = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=4,
            is_rational=False,
        )

        curve_geometry.set_knot(0, 1)
        curve_geometry.set_knot(1, 1)
        curve_geometry.set_knot(2, 2)
        curve_geometry.set_knot(3, 3)
        curve_geometry.set_knot(4, 3)

        curve_geometry.set_pole(0, [9.0, 6.0])
        curve_geometry.set_pole(1, [9.0, 7.0])
        curve_geometry.set_pole(2, [6.0, 7.0])
        curve_geometry.set_pole(3, [6.0, 8.0])

        intersections = an.curve_span_intersection(curve_geometry,
            knots_u=[5, 5, 6, 7, 8, 9, 10, 10],
            knots_v=[5, 5, 6, 8, 8, 10, 10],
            tolerance=1e-7,
            include_curve_knots=True,
        )

        assert_array_almost_equal(
            intersections,
            [1, 1.8164965809, 2, 2.1835034191, 3],
        )


if __name__ == '__main__':
    unittest.main()
