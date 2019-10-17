import unittest
import ANurbs as an
import numpy as np
from numpy.testing import assert_array_almost_equal, assert_equal, assert_array_equal

class TestRTree(unittest.TestCase):
    def test_example_2d(self):
        data = [[( 8, 62), (11, 66)],
                [(57, 17), (57, 19)],
                [(76, 26), (79, 29)],
                [(36, 56), (38, 56)],
                [(92, 77), (96, 80)],
                [(87, 70), (90, 74)],
                [(43, 41), (47, 43)],
                [( 0, 58), ( 2, 62)],
                [(76, 86), (80, 89)],
                [(27, 13), (27, 15)],
                [(71, 63), (75, 67)],
                [(25,  2), (27,  2)],
                [(87,  6), (88,  6)],
                [(22, 90), (23, 93)],
                [(22, 89), (22, 93)],
                [(57, 11), (61, 13)],
                [(61, 55), (63, 56)],
                [(17, 85), (21, 87)],
                [(33, 43), (37, 43)],
                [( 6,  1), ( 7,  3)],
                [(80, 87), (80, 87)],
                [(23, 50), (26, 52)],
                [(58, 89), (58, 89)],
                [(12, 30), (15, 34)],
                [(32, 58), (36, 61)],
                [(41, 84), (44, 87)],
                [(44, 18), (44, 19)],
                [(13, 63), (15, 67)],
                [(52, 70), (54, 74)],
                [(57, 59), (58, 59)],
                [(17, 90), (20, 92)],
                [(48, 53), (52, 56)],
                [(92, 68), (92, 72)],
                [(26, 52), (30, 52)],
                [(56, 23), (57, 26)],
                [(88, 48), (88, 48)],
                [(66, 13), (67, 15)],
                [( 7, 82), ( 8, 86)],
                [(46, 68), (50, 68)],
                [(37, 33), (38, 36)],
                [( 6, 15), ( 8, 18)],
                [(85, 36), (89, 38)],
                [(82, 45), (84, 48)],
                [(12,  2), (16,  3)],
                [(26, 15), (26, 16)],
                [(55, 23), (59, 26)],
                [(76, 37), (79, 39)],
                [(86, 74), (90, 77)],
                [(16, 75), (18, 78)],
                [(44, 18), (45, 21)],
                [(52, 67), (54, 71)],
                [(59, 78), (62, 78)],
                [(24,  5), (24,  8)],
                [(64, 80), (64, 83)],
                [(66, 55), (70, 55)],
                [( 0, 17), ( 2, 19)],
                [(15, 71), (18, 74)],
                [(87, 57), (87, 59)],
                [( 6, 34), ( 7, 37)],
                [(34, 30), (37, 32)],
                [(51, 19), (53, 19)],
                [(72, 51), (73, 55)],
                [(29, 45), (30, 45)],
                [(94, 94), (96, 95)],
                [( 7, 22), (11, 24)],
                [(86, 45), (87, 48)],
                [(33, 62), (34, 65)],
                [(18, 10), (21, 14)],
                [(64, 66), (67, 67)],
                [(64, 25), (65, 28)],
                [(27,  4), (31,  6)],
                [(84,  4), (85,  5)],
                [(48, 80), (50, 81)],
                [( 1, 61), ( 3, 61)],
                [(71, 89), (74, 92)],
                [(40, 42), (43, 43)],
                [(27, 64), (28, 66)],
                [(46, 26), (50, 26)],
                [(53, 83), (57, 87)],
                [(14, 75), (15, 79)],
                [(31, 45), (34, 45)],
                [(89, 84), (92, 88)],
                [(84, 51), (85, 53)],
                [(67, 87), (67, 89)],
                [(39, 26), (43, 27)],
                [(47, 61), (47, 63)],
                [(23, 49), (25, 53)],
                [(12,  3), (14,  5)],
                [(16, 50), (19, 53)],
                [(63, 80), (64, 84)],
                [(22, 63), (22, 64)],
                [(26, 66), (29, 66)],
                [( 2, 15), ( 3, 15)],
                [(74, 77), (77, 79)],
                [(64, 11), (68, 11)],
                [(38,  4), (39,  8)],
                [(83, 73), (87, 77)],
                [(85, 52), (89, 56)],
                [(74, 60), (76, 63)],
                [(62, 66), (65, 67)]]

        index = an.RTree2D(len(data))

        for box_a, box_b in data:
            index.add(box_a, box_b)

        index.finish()
        
        assert_equal(index.nb_items, 100)
        assert_equal(index.node_size, 16)
        assert_equal(len(index.indices), 108)

        assert_equal(len(index._boxes_min), 108)
        assert_equal(len(index._boxes_max), 108)

        assert_equal(index.indices[-1], 100)

        assert_equal(index._boxes_min[-1], [0, 1])
        assert_equal(index._boxes_max[-1], [96, 95])

        ids = index.search([40, 40], [60, 60], None)

        assert_array_equal(np.sort(ids), [6, 29, 31, 75])

    def create_tree_3d(self):
        data = [[(-28, -35, -20), (-17, -27,  -7)],
                [(-53, -55,  59), (-49, -47,  79)],
                [( 22, -19,  55), ( 35,  -4,  58)],
                [( 18, -82,   8), ( 37, -70,  12)],
                [( 20,  -8, -22), ( 36,  -6,  -2)],
                [(-25,  -7,  18), (-17,   6,  36)],
                [( -8, -49,  40), (  4, -39,  48)],
                [(-50, -54,   9), (-48, -36,  11)],
                [(-57, -26,  31), (-46, -20,  32)],
                [(-67,   5, -12), (-47,  10,  -2)],
                [( 13, -34, -38), ( 24, -34, -28)],
                [(-32, -69, -19), (-13, -57, -15)],
                [(-21,   1, -27), ( -1,   9, -22)],
                [( 18, -82,  54), ( 18, -63,  56)],
                [( 19, -12,  26), ( 24,   5,  37)],
                [(-56,   2,  59), (-47,  18,  60)],
                [(-14, -69,   9), ( -6, -54,  11)],
                [(-13, -18,   6), ( -2,  -4,   7)],
                [(-12, -65,  59), ( -3, -55,  70)],
                [( 21, -55,  -5), ( 37, -54,  10)],
                [(-67, -37,  -5), (-50, -26,   1)],
                [( 21, -29,  19), ( 27, -26,  32)],
                [(-47, -55,  35), (-40, -55,  42)],
                [(  9, -27,  44), ( 18, -10,  47)],
                [( 39, -32, -38), ( 55, -12, -22)],
                [( 39, -47,  33), ( 52, -45,  49)],
                [(  1, -69,  26), (  7, -68,  31)],
                [(-44,   5,   6), (-27,   7,  19)],
                [(  0, -70, -16), ( 16, -65, -10)],
                [( 15, -16, -20), ( 23, -16, -12)],
                [(-49, -73, -30), (-32, -65, -23)],
                [(-26, -10, -28), ( -7,   0, -28)],
                [(-39,  -4,   5), (-23,  -4,  21)],
                [( -6, -60, -12), ( 12, -43,   4)],
                [(-29, -15,  42), (-23,  -7,  60)],
                [( 32,   9, -24), ( 46,  25,  -7)],
                [(  5,   6,  -4), ( 12,   7,   2)],
                [( 40, -59,   2), ( 50, -56,  18)],
                [(-26, -35,   4), (-19, -21,  23)],
                [(  4, -48,  26), ( 20, -42,  37)],
                [(-50, -38,  -1), (-32, -26,   2)],
                [( 19, -30,  -4), ( 28, -28,   9)],
                [(-59,  -6,  -2), (-50,   7,   2)],
                [(-17, -17,  39), (-17, -12,  48)],
                [( 28,   5,  37), ( 46,   5,  56)],
                [(  3, -25,  21), (  5, -22,  28)],
                [(-28, -50, -27), ( -9, -44, -19)],
                [(-42, -53, -14), (-42, -53, -13)],
                [(-38, -37,  44), (-32, -35,  46)],
                [(  0,   3,  21), (  1,   8,  31)],
                [( 31, -68,  38), ( 37, -58,  53)],
                [(-18, -57,  60), (-13, -40,  61)],
                [(-28, -26, -12), (-11, -14, -11)],
                [(-50, -26, -34), (-34, -10, -26)],
                [(-29, -69,  21), (-25, -57,  41)],
                [( -6, -55,  -9), ( -3, -47,  10)],
                [( 16, -72, -29), ( 33, -60, -22)],
                [(-34, -66,  40), (-31, -55,  55)],
                [( 22, -22,  16), ( 38, -10,  23)],
                [(-17, -19,  52), ( -9,  -3,  69)],
                [( 20,  -6,  10), ( 26,   1,  17)],
                [( 15,  -8, -30), ( 30,   0, -29)],
                [(-34,   3,  51), (-28,   6,  63)],
                [(-49, -39,  25), (-48, -25,  38)],
                [(-58, -34, -28), (-41, -17, -11)],
                [(-10, -46,   4), (  1, -36,  13)],
                [( 17, -54,  59), ( 34, -46,  66)],
                [(-17, -61,  27), (  2, -56,  35)],
                [(-41, -73, -10), (-28, -67, -10)],
                [(-47, -39,  55), (-46, -27,  69)],
                [(-16,   6, -23), (-12,  10, -10)],
                [( 28, -29,  37), ( 46, -21,  40)],
                [( 30,  -6,  36), ( 32,  14,  38)],
                [( 11, -47,  47), ( 17, -31,  49)],
                [(-12, -53,  38), (  6, -46,  47)],
                [( 18,  10,  43), ( 30,  17,  44)],
                [(-45, -15,  23), (-28,  -5,  26)],
                [(-15,   7,  17), ( -2,  22,  35)],
                [( 15, -38, -16), ( 26, -34, -13)],
                [(-46, -13,  59), (-40, -11,  70)],
                [( 35, -14,   6), ( 47,   2,  10)],
                [( 25, -32, -26), ( 38, -12, -12)],
                [(  7,  -9,  59), ( 10,   7,  64)],
                [(  7, -52,  25), ( 12, -42,  41)],
                [(-29, -28,  48), (-11, -18,  51)],
                [(-44, -28,  -9), (-36, -17,   5)],
                [( -3, -45,  59), ( 13, -43,  78)],
                [(-45, -60,   6), (-33, -56,  10)],
                [(-49, -48,  -1), (-47, -39,  15)],
                [(-31, -70, -47), (-15, -51, -29)],
                [(-24, -49, -11), (-13, -41,  -6)],
                [( 17,  -8, -26), ( 27,   1, -14)],
                [( 24, -66,  24), ( 30, -55,  35)],
                [( 10, -53, -41), ( 14, -43, -26)],
                [( -4, -60,  17), ( -2, -40,  19)],
                [(-55, -53, -47), (-42, -37, -29)],
                [(-26, -52, -47), ( -9, -33, -28)],
                [( 17,   9,   1), ( 26,  28,  11)],
                [( 34, -40,  13), ( 37, -39,  19)],
                [(-25, -79,  -7), (-12, -68,   2)]]

        index = an.RTree3D(len(data))

        for box_a, box_b in data:
            index.add(box_a, box_b)

        index.finish()

        return index

    def test_create_3d(self):
        index = self.create_tree_3d()

        assert_equal(index.nb_items, 100)
        assert_equal(index.node_size, 16)
        assert_equal(len(index.indices), 108)

        assert_equal(len(index._boxes_min), 108)
        assert_equal(len(index._boxes_max), 108)

        assert_equal(index.indices[-1], 100)

        assert_equal(index._boxes_min[-1], [-67, -82, -47])
        assert_equal(index._boxes_max[-1], [ 55,  28,  79])

    def test_search_3d(self):
        index = self.create_tree_3d()

        ids = index.search([-24, -52, -11], [24, -12,  37], None)

        assert_array_equal(np.sort(ids), [0, 14, 17, 21, 33, 38, 39, 41, 45, 52, 55, 58, 65, 83, 90, 94])

    def test_search_ray_intersection_3d(self):
        index = self.create_tree_3d()

        ids = index.search_ray_intersection([33, -26, -22], [-82, -13, 60], None)

        assert_array_equal(np.sort(ids), [38, 63, 81])

if __name__ == '__main__':
    unittest.main()
