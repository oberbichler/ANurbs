import unittest
import anurbs as an

class TestModel(unittest.TestCase):
    def test_load_array(self):
        model = an.Model()

        model.add_array(r"""[
            {
                "Key": "TestData",
                "Type": "NurbsCurveGeometry2D",
                "Degree": 1,
                "NbPoles": 2,
                "Knots": [0, 1],
                "Poles": [[1, 2], [3, 4]]
            }
        ]""")

        ptr = model.get(0)

        self.assertEqual(ptr.key, 'TestData')
        self.assertTrue(isinstance(ptr.data, an.NurbsCurveGeometry2D))


if __name__ == '__main__':
    unittest.main()
