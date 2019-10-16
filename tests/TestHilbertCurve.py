import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_almost_equal, assert_array_equal

class TestHilbertCurve(unittest.TestCase):
    def test_rotl_2d(self):
        b = lambda bin_str: int(bin_str, 2)

        actual = an.HilbertCurve2D._rol(b('01'), 0)
        self.assertEqual(actual, b('01'))

        actual = an.HilbertCurve2D._rol(b('01'), 1)
        self.assertEqual(actual, b('10'))

        actual = an.HilbertCurve2D._rol(b('01'), 2)
        self.assertEqual(actual, b('01'))

        actual = an.HilbertCurve2D._rol(b('01'), 3)
        self.assertEqual(actual, b('10'))

        actual = an.HilbertCurve2D._rol(b('01'), -1)
        self.assertEqual(actual, b('10'))

        actual = an.HilbertCurve2D._rol(b('01'), -2)
        self.assertEqual(actual, b('01'))

        actual = an.HilbertCurve2D._rol(b('01'), -3)
        self.assertEqual(actual, b('10'))

    def test_rotl_3d(self):
        b = lambda bin_str: int(bin_str, 2)

        actual = an.HilbertCurve3D._rol(b('001'), 0)
        self.assertEqual(actual, b('001'))

        actual = an.HilbertCurve3D._rol(b('001'), 1)
        self.assertEqual(actual, b('010'))

        actual = an.HilbertCurve3D._rol(b('001'), 2)
        self.assertEqual(actual, b('100'))

        actual = an.HilbertCurve3D._rol(b('001'), 3)
        self.assertEqual(actual, b('001'))

        actual = an.HilbertCurve3D._rol(b('001'), 4)
        self.assertEqual(actual, b('010'))

        actual = an.HilbertCurve3D._rol(b('001'), -1)
        self.assertEqual(actual, b('100'))

        actual = an.HilbertCurve3D._rol(b('001'), -2)
        self.assertEqual(actual, b('010'))

        actual = an.HilbertCurve3D._rol(b('001'), -3)
        self.assertEqual(actual, b('001'))

        actual = an.HilbertCurve3D._rol(b('001'), -4)
        self.assertEqual(actual, b('100'))

    def test_rotr_2d(self):
        b = lambda bin_str: int(bin_str, 2)

        actual = an.HilbertCurve2D._ror(b('01'), 0)
        self.assertEqual(actual, b('01'))

        actual = an.HilbertCurve2D._ror(b('01'), -1)
        self.assertEqual(actual, b('10'))

        actual = an.HilbertCurve2D._ror(b('01'), -2)
        self.assertEqual(actual, b('01'))

        actual = an.HilbertCurve2D._ror(b('01'), -3)
        self.assertEqual(actual, b('10'))

        actual = an.HilbertCurve2D._ror(b('01'), 1)
        self.assertEqual(actual, b('10'))

        actual = an.HilbertCurve2D._ror(b('01'), 2)
        self.assertEqual(actual, b('01'))

        actual = an.HilbertCurve2D._ror(b('01'), 3)
        self.assertEqual(actual, b('10'))

    def test_rotr_3d(self):
        b = lambda bin_str: int(bin_str, 2)

        actual = an.HilbertCurve3D._ror(b('001'), 0)
        self.assertEqual(actual, b('001'))

        actual = an.HilbertCurve3D._ror(b('001'), -1)
        self.assertEqual(actual, b('010'))

        actual = an.HilbertCurve3D._ror(b('001'), -2)
        self.assertEqual(actual, b('100'))

        actual = an.HilbertCurve3D._ror(b('001'), -3)
        self.assertEqual(actual, b('001'))

        actual = an.HilbertCurve3D._ror(b('001'), -4)
        self.assertEqual(actual, b('010'))

        actual = an.HilbertCurve3D._ror(b('001'), 1)
        self.assertEqual(actual, b('100'))

        actual = an.HilbertCurve3D._ror(b('001'), 2)
        self.assertEqual(actual, b('010'))

        actual = an.HilbertCurve3D._ror(b('001'), 3)
        self.assertEqual(actual, b('001'))

        actual = an.HilbertCurve3D._ror(b('001'), 4)
        self.assertEqual(actual, b('100'))

    def bit(self, hilbert_curve):
        bits = '01001'

        for i in range(5):
            actual = hilbert_curve._bit(int(bits, 2), i)
            self.assertEqual(actual, int(bits[-i - 1]))

    def test_bit_2d(self):
        self.bit(an.HilbertCurve2D)

    def test_bit_3d(self):
        self.bit(an.HilbertCurve3D)

    def e(self, hilbert_curve):
        expected = [0, 0, 0, 3, 3, 6, 6, 5, 5, 12, 12, 15, 15, 10, 10, 9, 9, 24, 24, 27, 27, 30, 30, 29, 29, 20, 20, 23, 23, 18, 18, 17, 17, 48, 48, 51, 51, 54, 54, 53, 53, 60, 60, 63, 63, 58, 58, 57, 57, 40, 40, 43, 43, 46, 46, 45, 45, 36, 36, 39, 39, 34, 34, 33]
        actual = [hilbert_curve._e(i) for i in range(64)]
        self.assertEqual(actual, expected)

    def test_e_2d(self):
        self.e(an.HilbertCurve2D)

    def test_e_3d(self):
        self.e(an.HilbertCurve3D)

    def g(self, hilbert_curve):
        expected = [0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6]
        actual = [hilbert_curve._g(i) for i in range(64)]
        self.assertEqual(actual, expected)

    def test_g_2d(self):
        self.g(an.HilbertCurve2D)

    def test_g_3d(self):
        self.g(an.HilbertCurve3D)

    def gc(self, hilbert_curve):
        expected = [0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8, 24, 25, 27, 26, 30, 31, 29, 28, 20, 21, 23, 22, 18, 19, 17, 16, 48, 49, 51, 50, 54, 55, 53, 52, 60, 61, 63, 62, 58, 59, 57, 56, 40, 41, 43, 42, 46, 47, 45, 44, 36, 37, 39, 38, 34, 35, 33, 32]
        actual = [hilbert_curve._gc(i) for i in range(64)]
        self.assertEqual(actual, expected)

    def test_gc_2d(self):
        self.gc(an.HilbertCurve2D)

    def test_gc_3d(self):
        self.gc(an.HilbertCurve3D)

    def test_inverse_gc_2d(self):
        expected = [0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8, 24, 25, 27, 26, 30, 31, 29, 28, 20, 21, 23, 22, 18, 19, 17, 16, 48, 49, 51, 50, 54, 55, 53, 52, 60, 61, 63, 62, 58, 59, 57, 56, 40, 41, 43, 42, 46, 47, 45, 44, 36, 37, 39, 38, 34, 35, 33, 32]
        actual = [an.HilbertCurve2D._inverse_gc(i) for i in range(64)]
        self.assertEqual(actual, expected)

    def test_inverse_gc_3d(self):
        expected = [0, 1, 3, 2, 7, 6, 4, 5, 14, 15, 13, 12, 9, 8, 10, 11, 28, 29, 31, 30, 27, 26, 24, 25, 18, 19, 17, 16, 21, 20, 22, 23, 56, 57, 59, 58, 63, 62, 60, 61, 54, 55, 53, 52, 49, 48, 50, 51, 36, 37, 39, 38, 35, 34, 32, 33, 42, 43, 41, 40, 45, 44, 46, 47]
        actual = [an.HilbertCurve3D._inverse_gc(i) for i in range(64)]
        self.assertEqual(actual, expected)

    def test_t_2d(self):
        data = [
            ([0, 0, 0], 0),
            ([0, -1, 0], 0),
            ([0, 0, 0], 0),
            ([0, -1, 1], 1),
            ([0, 0, 0], 0),
            ([0, -1, 3], 3),
            ([0, 0, 0], 0),
            ([0, -1, 2], 2),
            ([0, 0, 1], 2),
            ([0, 0, 0], 0),
            ([0, 0, 1], 2),
            ([0, 0, 1], 2),
            ([0, 0, 1], 2),
            ([0, 0, 3], 3),
            ([0, 0, 1], 2),
            ([0, 0, 2], 1),
            ([0, 0, 3], 3),
            ([0, 0, 0], 0),
            ([0, 0, 3], 3),
            ([0, 0, 1], 2),
            ([0, 0, 3], 3),
            ([0, 0, 3], 3),
            ([0, 0, 3], 3),
            ([0, 0, 2], 1),
            ([0, 0, 2], 1),
            ([3, -1, 0], 3),
            ([0, 0, 2], 1),
            ([3, -1, 1], 2),
            ([0, 0, 2], 1),
            ([3, -1, 3], 0),
            ([0, 0, 2], 1),
            ([3, -1, 2], 1),
            ([0, 0, 2], 1),
            ([0, 0, 1], 2),
        ]

        for (e, d, b), expected in data:
            actual = an.HilbertCurve2D._t(e, d, b)
            self.assertEqual(actual, expected)

    def test_t_3d(self):
        data = [
            ((0, 1, 0), 0),
            ((0, 0, 0), 0),
            ((0, 1, 0), 0),
            ((0, 0, 1), 4),
            ((0, 1, 0), 0),
            ((0, 0, 3), 5),
            ((0, 1, 0), 0),
            ((0, 0, 2), 1),
            ((0, 1, 0), 0),
            ((0, 0, 6), 3),
            ((0, 1, 0), 0),
            ((0, 0, 7), 7),
            ((0, 1, 0), 0),
            ((0, 0, 5), 6),
            ((0, 1, 0), 0),
            ((0, 0, 4), 2),
            ((0, 1, 1), 2),
            ((0, -1, 0), 0),
            ((0, 1, 1), 2),
            ((0, -1, 1), 1),
            ((0, 1, 1), 2),
            ((0, -1, 3), 3),
            ((0, 1, 1), 2),
            ((0, -1, 2), 2),
            ((0, 1, 1), 2),
            ((0, -1, 6), 6),
            ((0, 1, 1), 2),
            ((0, -1, 7), 7),
            ((0, 1, 1), 2),
            ((0, -1, 5), 5),
            ((0, 1, 1), 2),
            ((0, -1, 4), 4),
            ((0, 1, 3), 6),
            ((0, -1, 0), 0),
            ((0, 1, 3), 6),
            ((0, -1, 1), 1),
            ((0, 1, 3), 6),
            ((0, -1, 3), 3),
            ((0, 1, 3), 6),
            ((0, -1, 2), 2),
            ((0, 1, 3), 6),
            ((0, -1, 6), 6),
            ((0, 1, 3), 6),
            ((0, -1, 7), 7),
            ((0, 1, 3), 6),
            ((0, -1, 5), 5),
            ((0, 1, 3), 6),
            ((0, -1, 4), 4),
            ((0, 1, 2), 4),
            ((3, 1, 0), 6),
            ((0, 1, 2), 4),
            ((3, 1, 1), 4),
            ((0, 1, 2), 4),
            ((3, 1, 3), 0),
            ((0, 1, 2), 4),
            ((3, 1, 2), 2),
            ((0, 1, 2), 4),
            ((3, 1, 6), 3),
            ((0, 1, 2), 4),
            ((3, 1, 7), 1),
            ((0, 1, 2), 4),
            ((3, 1, 5), 5),
            ((0, 1, 2), 4),
            ((3, 1, 4), 7),
            ((0, 1, 6), 5),
            ((3, 1, 0), 6),
            ((0, 1, 6), 5),
            ((3, 1, 1), 4),
            ((0, 1, 6), 5),
            ((3, 1, 3), 0),
            ((0, 1, 6), 5),
            ((3, 1, 2), 2),
            ((0, 1, 6), 5),
            ((3, 1, 6), 3),
            ((0, 1, 6), 5),
            ((3, 1, 7), 1),
            ((0, 1, 6), 5),
            ((3, 1, 5), 5),
            ((0, 1, 6), 5),
            ((3, 1, 4), 7),
            ((0, 1, 7), 7),
            ((5, -1, 0), 5),
            ((0, 1, 7), 7),
            ((5, -1, 1), 4),
            ((0, 1, 7), 7),
            ((5, -1, 3), 6),
            ((0, 1, 7), 7),
            ((5, -1, 2), 7),
            ((0, 1, 7), 7),
            ((5, -1, 6), 3),
            ((0, 1, 7), 7),
            ((5, -1, 7), 2),
            ((0, 1, 7), 7),
            ((5, -1, 5), 0),
            ((0, 1, 7), 7),
            ((5, -1, 4), 1),
            ((0, 1, 5), 3),
            ((5, -1, 0), 5),
            ((0, 1, 5), 3),
            ((5, -1, 1), 4),
            ((0, 1, 5), 3),
            ((5, -1, 3), 6),
            ((0, 1, 5), 3),
            ((5, -1, 2), 7),
            ((0, 1, 5), 3),
            ((5, -1, 6), 3),
            ((0, 1, 5), 3),
            ((5, -1, 7), 2),
            ((0, 1, 5), 3),
            ((5, -1, 5), 0),
            ((0, 1, 5), 3),
            ((5, -1, 4), 1),
            ((0, 1, 4), 1),
            ((6, 0, 0), 3),
            ((0, 1, 4), 1),
            ((6, 0, 1), 7),
            ((0, 1, 4), 1),
            ((6, 0, 3), 6),
            ((0, 1, 4), 1),
            ((6, 0, 2), 2),
            ((0, 1, 4), 1),
            ((6, 0, 6), 0),
            ((0, 1, 4), 1),
            ((6, 0, 7), 4),
            ((0, 1, 4), 1),
            ((6, 0, 5), 5),
            ((0, 1, 4), 1),
            ((6, 0, 4), 1),
        ]

        for (e, d, b), expected in data:
            actual = an.HilbertCurve3D._t(e, d, b)
            self.assertEqual(actual, expected)

    def test_inverse_t_3d(self):
        data = [
            ((0, 0, 0), 0),
            ((0, 1, 0), 0),
            ((0, 0, 0), 0),
            ((0, 1, 1), 1),
            ((0, 0, 0), 0),
            ((0, 1, 3), 3),
            ((0, 0, 0), 0),
            ((0, 1, 2), 2),
            ((0, 0, 1), 2),
            ((0, 0, 0), 0),
            ((0, 0, 1), 2),
            ((0, 0, 1), 2),
            ((0, 0, 1), 2),
            ((0, 0, 3), 3),
            ((0, 0, 1), 2),
            ((0, 0, 2), 1),
            ((0, 0, 3), 3),
            ((0, 0, 0), 0),
            ((0, 0, 3), 3),
            ((0, 0, 1), 2),
            ((0, 0, 3), 3),
            ((0, 0, 3), 3),
            ((0, 0, 3), 3),
            ((0, 0, 2), 1),
            ((0, 0, 2), 1),
            ((3, 1, 0), 3),
            ((0, 0, 2), 1),
            ((3, 1, 1), 2),
            ((0, 0, 2), 1),
            ((3, 1, 3), 0),
            ((0, 0, 2), 1),
            ((3, 1, 2), 1),
        ]

        for (e, d, b), expected in data:
            actual = an.HilbertCurve2D._inverse_t(e, d, b)
            self.assertEqual(actual, expected)

    def test_evaluate_2d(self):
        expected = [
            (0, 0),
            (1, 0),
            (1, 1),
            (0, 1),
            (0, 2),
            (0, 3),
            (1, 3),
            (1, 2),
            (2, 2),
            (2, 3),
            (3, 3),
            (3, 2),
            (3, 1),
            (2, 1),
            (2, 0),
            (3, 0),
        ]

        actual = [an.HilbertCurve2D.point_at(2, i) for i in range(16)]

        assert_array_equal(actual, expected)

    def test_evaluate_3d(self):
        expected = [
            (0, 0, 0),
            (0, 0, 1),
            (1, 0, 1),
            (1, 0, 0),
            (1, 1, 0),
            (1, 1, 1),
            (0, 1, 1),
            (0, 1, 0),
            (0, 2, 0),
            (1, 2, 0),
            (1, 3, 0),
            (0, 3, 0),
            (0, 3, 1),
            (1, 3, 1),
            (1, 2, 1),
            (0, 2, 1),
            (0, 2, 2),
            (1, 2, 2),
            (1, 3, 2),
            (0, 3, 2),
            (0, 3, 3),
            (1, 3, 3),
            (1, 2, 3),
            (0, 2, 3),
            (0, 1, 3),
            (0, 0, 3),
            (0, 0, 2),
            (0, 1, 2),
            (1, 1, 2),
            (1, 0, 2),
            (1, 0, 3),
            (1, 1, 3),
            (2, 1, 3),
            (2, 0, 3),
            (2, 0, 2),
            (2, 1, 2),
            (3, 1, 2),
            (3, 0, 2),
            (3, 0, 3),
            (3, 1, 3),
            (3, 2, 3),
            (2, 2, 3),
            (2, 3, 3),
            (3, 3, 3),
            (3, 3, 2),
            (2, 3, 2),
            (2, 2, 2),
            (3, 2, 2),
            (3, 2, 1),
            (2, 2, 1),
            (2, 3, 1),
            (3, 3, 1),
            (3, 3, 0),
            (2, 3, 0),
            (2, 2, 0),
            (3, 2, 0),
            (3, 1, 0),
            (3, 1, 1),
            (2, 1, 1),
            (2, 1, 0),
            (2, 0, 0),
            (2, 0, 1),
            (3, 0, 1),
            (3, 0, 0),
        ]

        actual = [an.HilbertCurve3D.point_at(2, i) for i in range(64)]

        assert_array_equal(actual, expected)

    def test_project_2d(self):
        for i in range(16):
            p = an.HilbertCurve2D.point_at(2, i)
            h = an.HilbertCurve2D.index_at(2, p)
            self.assertEqual(h, i)

    def test_project_3d(self):
        for i in range(64):
            p = an.HilbertCurve3D.point_at(2, i)
            h = an.HilbertCurve3D.index_at(2, p)
            self.assertEqual(h, i)

    def test_end_point_2d(self):
        n = 2
        for m in range(32):
            p = an.HilbertCurve2D.point_at(m, 2**(n * m) - 1)
            assert_array_equal(p, [2**m - 1, 0])

    def test_end_point_3d(self):
        n = 3
        for m in range(22):
            p = an.HilbertCurve3D.point_at(m, 2**(n * m) - 1)
            assert_array_equal(p, [2**m - 1, 0, 0])

if __name__ == '__main__':
    unittest.main()
