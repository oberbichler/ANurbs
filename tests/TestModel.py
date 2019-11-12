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

    def test_to_string(self):
        model = an.Model()

        model.add(key='TestData', data=an.NurbsCurveGeometry2D(degree=1, knots=[0, 1], poles=[[1, 2], [3, 4]]))

        actual = model.to_string()
        expected = r"""[
                      {
                        "Key": "TestData",
                        "Type": "NurbsCurveGeometry2D",
                        "Degree": 1,
                        "Knots": [0.0,1.0],
                        "NbPoles": 2,
                        "Poles": [[1.0,2.0],[3.0,4.0]]
                      }
                    ]"""

        for actual_line, expected_line in zip(actual.split('\n'), expected.split('\n')):
            self.assertAlmostEqual(actual_line.strip(), expected_line.strip())

if __name__ == '__main__':
    unittest.main()
