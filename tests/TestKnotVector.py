import unittest
import ANurbs as an

class TestKnotVector(unittest.TestCase):
    def test_knot_vector(self):
        knots = [0, 0, 0, 1, 2, 2, 3, 3, 3]

        self.assertEqual(an.upper_span(3, knots, 0), 2)
        self.assertEqual(an.upper_span(3, knots, 1), 3)
        self.assertEqual(an.upper_span(3, knots, 2), 5)
        self.assertEqual(an.upper_span(3, knots, 3), 5)

        self.assertEqual(an.lower_span(3, knots, 0), 2)
        self.assertEqual(an.lower_span(3, knots, 1), 2)
        self.assertEqual(an.lower_span(3, knots, 2), 3)
        self.assertEqual(an.lower_span(3, knots, 3), 5)

        degree = 3
        nb_knots = 8
        nb_poles = 6

        self.assertEqual(an.degree(nb_knots, nb_poles), degree)

        self.assertEqual(an.nb_knots(degree, nb_poles), nb_knots)

        self.assertEqual(an.nb_poles(degree, nb_knots), nb_poles)

        self.assertEqual(an.nb_spans(degree, nb_knots), 3)
