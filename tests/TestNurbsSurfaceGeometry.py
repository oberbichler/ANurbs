import anurbs as an
import pytest
import os
from numpy.testing import assert_almost_equal, assert_equal

if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)


@pytest.fixture
def dome():
    model = an.Model()
    model.load(f'{os.path.dirname(__file__)}/data/dome.ibra')
    return model.of_type('NurbsSurfaceGeometry3D')[0][1]


@pytest.fixture
def bspline_3d():
    surface = an.NurbsSurfaceGeometry3D(
        degree_u=2,
        degree_v=1,
        nb_poles_u=4,
        nb_poles_v=3,
        is_rational=False,
    )

    surface.knots_u = [0, 0, 7.5, 15, 15]
    surface.knots_v = [0, 10, 20]

    surface.poles = [
        [-10.0, -5.0, -1.0],
        [-12.0, 3.0, 3.0],
        [-9.0, 11.0, -0.0701928417],
        [-5.0, -3.0, 1.0],
        [-6.0, 4.0, -2.0],
        [-5.0, 7.0, 0.9298071583],
        [0.0, -4.0, -1.0],
        [1.0, 6.0, 5.0],
        [0.0, 13.0, -0.2350184214],
        [4.0, -2.0, 0.0],
        [5.0, 4.0, -1.0],
        [5.0, 11.0, 0.7649815786],
    ]

    return surface


def test_bspline_dimension(bspline_3d):
    assert_equal(bspline_3d.dimension, 3)


def test_bspline_degree_u(bspline_3d):
    assert_equal(bspline_3d.degree_u, 2)


def test_bspline_degree_v(bspline_3d):
    assert_equal(bspline_3d.degree_v, 1)


def test_bspline_nb_knots_u(bspline_3d):
    assert_equal(bspline_3d.nb_knots_u, 5)


def test_bspline_nb_knots_v(bspline_3d):
    assert_equal(bspline_3d.nb_knots_v, 3)


def test_bspline_nb_poles_u(bspline_3d):
    assert_equal(bspline_3d.nb_poles_u, 4)


def test_bspline_nb_poles_v(bspline_3d):
    assert_equal(bspline_3d.nb_poles_v, 3)


def test_bspline_nb_poles(bspline_3d):
    assert_equal(bspline_3d.nb_poles, 12)


def test_bspline_point_at(bspline_3d):
    point = bspline_3d.point_at(u=12, v=5)
    assert_almost_equal(point, [1.46, 0.96, 0.9])


def test_bspline_derivatives_at(bspline_3d):
    derivatives = bspline_3d.derivatives_at(u=12, v=5, order=2)
    assert_almost_equal(derivatives, [
        [1.46, 0.96, 0.9],
        [0.96, 0.0266666667, -0.2666666667],
        [0.084, 0.832, 0.276],
        [0.0355555556, -0.0088888889, -0.1333333333],
        [0.0106666667, -0.048, -0.064],
        [0, 0, 0],
    ])


def test_bspline_normal_at(bspline_3d):
    normal = bspline_3d.normal_at(u=12, v=5)
    assert_almost_equal(normal, [0.2613127124, -0.3275832699, 0.9079674375])


def test_bspline_list_constructor():
    an.NurbsSurfaceGeometry3D(
        degree_u=2,
        degree_v=1,
        knots_u=[0.0, 0.0, 7.5, 15.0, 15.0],
        knots_v=[0.0, 10.0, 20.0],
        poles=[
            [-10.0, -5.0, -1.0],
            [-12.0, 3.0, 3.0],
            [-9.0, 11.0, -0.0701928417],
            [-5.0, -3.0, 1.0],
            [-6.0, 4.0, -2.0],
            [-5.0, 7.0, 0.9298071583],
            [0.0, -4.0, -1.0],
            [1.0, 6.0, 5.0],
            [0.0, 13.0, -0.2350184214],
            [4.0, -2.0, 0.0],
            [5.0, 4.0, -1.0],
            [5.0, 11.0, 0.7649815786],
        ],
    )


def test_bspline_greville_point(bspline_3d):
    uv = bspline_3d.greville_point(index_u=1, index_v=1)
    assert_almost_equal(uv, [3.75, 10])


@pytest.fixture
def nurbs_3d():
    surface = an.NurbsSurfaceGeometry3D(
        degree_u=2,
        degree_v=1,
        nb_poles_u=4,
        nb_poles_v=3,
        is_rational=True,
    )

    surface.knots_u = [0, 0, 7.5, 15, 15]
    surface.knots_v = [0, 10, 20]

    surface.poles = [
        [-10.0, -5.0, -1.0],
        [-12.0, 3.0, 3.0],
        [- 9.0, 11.0, -0.0701928417],
        [- 5.0, -3.0, 1.0],
        [- 6.0, 4.0, -2.0],
        [- 5.0, 7.0, 0.9298071583],
        [0.0, -4.0, -1.0],
        [1.0, 6.0, 5.0],
        [0.0, 13.0, -0.2350184214],
        [4.0, -2.0, 0.0],
        [5.0, 4.0, -1.0],
        [5.0, 11.0, 0.7649815786],
    ]

    surface.weights = [1, 1, 1, 1, 2.5, 1, 1, 1, 1, 1, 1, 1]

    return surface


def test_nurbs_dimension(nurbs_3d):
    assert_equal(nurbs_3d.dimension, 3)


def test_nurbs_degree_u(nurbs_3d):
    assert_equal(nurbs_3d.degree_u, 2)


def test_nurbs_degree_v(nurbs_3d):
    assert_equal(nurbs_3d.degree_v, 1)


def test_nurbs_nb_knots_u(nurbs_3d):
    assert_equal(nurbs_3d.nb_knots_u, 5)


def test_nurbs_nb_knots_v(nurbs_3d):
    assert_equal(nurbs_3d.nb_knots_v, 3)


def test_nurbs_nb_poles_u(nurbs_3d):
    assert_equal(nurbs_3d.nb_poles_u, 4)


def test_nurbs_nb_poles_v(nurbs_3d):
    assert_equal(nurbs_3d.nb_poles_v, 3)


def test_nurbs_nb_poles(nurbs_3d):
    assert_equal(nurbs_3d.nb_poles, 12)


def test_nurbs_point_at(nurbs_3d):
    point = nurbs_3d.point_at(u=12, v=5)
    assert_almost_equal(point, [1.0377358491, 1.1320754717, 0.7358490566])


def test_nurbs_derivatives_at(nurbs_3d):
    derivatives = nurbs_3d.derivatives_at(u=12, v=5, order=2)
    assert_almost_equal(derivatives, [
        [1.0377358491, 1.1320754717, 0.7358490566],
        [1.1712353151, -0.0830663344, -0.1483327400],
        [-0.0004271983, 0.8173727305, 0.2294054824],
        [0.0334131158, 0.0214196208, -0.1713942830],
        [0.0499024922, -0.0351431047, -0.0293933919],
        [9.6724e-6, -0.0185065524, -0.0051940864],
    ])


def test_nurbs_normal_at(nurbs_3d):
    normal = nurbs_3d.normal_at(u=12, v=5)
    assert_almost_equal(normal, [0.1022371052, -0.2687554542, 0.9577672369])


def test_nurbs_greville_point(nurbs_3d):
    uv = nurbs_3d.greville_point(index_u=1, index_v=1)
    assert_almost_equal(uv, [3.75, 10])


def test_compute_at_span(dome):
    nonzero_indices_0, shape_functions_0 = dome.shape_functions_at_span(u=31.4159265358979, v=6.50645142284287, span_u=5, span_v=4, order=1)
    nonzero_indices_1, shape_functions_1 = dome.shape_functions_at_span(u=31.4159265358979, v=6.50645142284287, span_u=7, span_v=4, order=1)

    assert_almost_equal(shape_functions_0[0] @ dome.poles[nonzero_indices_0], [6, 0, 10])
    assert_almost_equal(shape_functions_1[0] @ dome.poles[nonzero_indices_1], [6, 0, 10])

    assert_almost_equal(shape_functions_0[1] @ dome.poles[nonzero_indices_0], [0, -0.72025305, 0.18006326])
    assert_almost_equal(shape_functions_0[2] @ dome.poles[nonzero_indices_0], [0.83958849, 0, -1.0494856])

    assert_almost_equal(shape_functions_1[1] @ dome.poles[nonzero_indices_1], [0, -0.72025305, -0.18006326])
    assert_almost_equal(shape_functions_1[2] @ dome.poles[nonzero_indices_1], [0.83958849, 0, -1.0494856])
