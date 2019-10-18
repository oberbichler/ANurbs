import unittest
import ANurbs as an
from numpy.testing import assert_array_almost_equal

class TestCurveTessellation(unittest.TestCase):
    def test_tessellation_2d(self):
        curve = an.NurbsCurveGeometry2D(
            degree=2,
            nb_poles=4,
            is_rational=False,
        )

        curve.set_knot(0, 0.0)
        curve.set_knot(1, 0.0)
        curve.set_knot(2, 1.0)
        curve.set_knot(3, 2.0)
        curve.set_knot(4, 2.0)

        curve.set_pole(0, [0.0,  0.0])
        curve.set_pole(1, [1.0,  1.0])
        curve.set_pole(2, [2.0, -1.0])
        curve.set_pole(3, [3.0, -1.0])

        tessellation = an.tessellate(curve, tolerance=1e-2)

        self.assertEqual(len(tessellation), 17)

        points = [point for _, point in tessellation]

        assert_array_almost_equal(
            points,
            [[0.0000000,  0.000000],
             [0.2421875,  0.218750],
             [0.4687500,  0.375000],
             [0.6796875,  0.468750],
             [0.8750000,  0.500000],
             [1.0546875,  0.468750],
             [1.2187500,  0.375000],
             [1.3671875,  0.218750],
             [1.5000000,  0.000000],
             [1.6328125, -0.234375],
             [1.7812500, -0.437500],
             [1.9453125, -0.609375],
             [2.1250000, -0.750000],
             [2.3203125, -0.859375],
             [2.5312500, -0.937500],
             [2.7578125, -0.984375],
             [3.0000000, -1.000000]]
        )

    def test_tessellation_3d(self):
        curve = an.NurbsCurveGeometry3D(
            degree=2,
            nb_poles=4,
            is_rational=False,
        )

        curve.set_knot(0, 0.0)
        curve.set_knot(1, 0.0)
        curve.set_knot(2, 1.0)
        curve.set_knot(3, 2.0)
        curve.set_knot(4, 2.0)

        curve.set_pole(0, [0.0,  0.0, -1.0])
        curve.set_pole(1, [1.0,  1.0,  1.0])
        curve.set_pole(2, [2.0, -1.0,  0.0])
        curve.set_pole(3, [3.0, -1.0,  0.0])

        tessellation = an.tessellate(curve, tolerance=1e-2)

        self.assertEqual(len(tessellation), 20)

        points = [point for _, point in tessellation]

        assert_array_almost_equal(
            points,
            [[0.0        ,  0.0      , -1.0        ],
             [0.2421875  ,  0.21875  , -0.5390625  ],
             [0.46875    ,  0.375    , -0.15625    ],
             [0.6796875  ,  0.46875  ,  0.1484375  ],
             [0.875      ,  0.5      ,  0.375      ],
             [0.966796875,  0.4921875,  0.458984375],
             [1.0546875  ,  0.46875  ,  0.5234375  ],
             [1.138671875,  0.4296875,  0.568359375],
             [1.21875    ,  0.375    ,  0.59375    ],
             [1.294921875,  0.3046875,  0.599609375],
             [1.3671875  ,  0.21875  ,  0.5859375  ],
             [1.5        ,  0.0      ,  0.5        ],
             [1.6328125  , -0.234375 ,  0.3828125  ],
             [1.78125    , -0.4375   ,  0.28125    ],
             [1.9453125  , -0.609375 ,  0.1953125  ],
             [2.125      , -0.75     ,  0.125      ],
             [2.3203125  , -0.859375 ,  0.0703125  ],
             [2.53125    ,-0.9375    ,  0.03125    ],
             [2.7578125  ,-0.984375  ,  0.0078125  ],
             [3.0        ,-1.0       ,  0.0        ]]
        )


if __name__ == '__main__':
    unittest.main()
