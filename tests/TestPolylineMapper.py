import unittest
import anurbs as an
from numpy.testing import assert_equal

class TestPolylineMapper(unittest.TestCase):
    @staticmethod
    def test_map_2d():
        polyline = [[0, 0], [1, 1], [2, 0]]

        mapper = an.PolylineMapper2D(polyline)

        t0, i0, t1, i1 = mapper.map([0.25, 0.25], tolerance=1e-4)

        assert_equal([t0, i0, t1, i1], [0.75, 0, 0.25, 1])

        t0, i0, t1, i1 = mapper.map([1.75, 0.25], tolerance=1e-4)

        assert_equal([t0, i0, t1, i1], [0.25, 1, 0.75, 2])

if __name__ == '__main__':
    unittest.main()
