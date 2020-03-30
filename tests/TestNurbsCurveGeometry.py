import anurbs as an
import numpy as np
import pytest
from numpy.testing import assert_almost_equal, assert_equal

if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)


@pytest.fixture
def bspline_3d():
    geometry = an.NurbsCurveGeometry3D(
        degree=4,
        nb_poles=8,
        is_rational=False,
    )

    geometry.set_knot(0, 0)
    geometry.set_knot(1, 0)
    geometry.set_knot(2, 0)
    geometry.set_knot(3, 0)
    geometry.set_knot(4, 32.9731425998736)
    geometry.set_knot(5, 65.9462851997473)
    geometry.set_knot(6, 98.9194277996209)
    geometry.set_knot(7, 131.892570399495)
    geometry.set_knot(8, 131.892570399495)
    geometry.set_knot(9, 131.892570399495)
    geometry.set_knot(10, 131.892570399495)

    geometry.set_pole(0, [0, -25, -5])
    geometry.set_pole(1, [-15, -15, 0])
    geometry.set_pole(2, [5, -5, -3])
    geometry.set_pole(3, [15, -15, 3])
    geometry.set_pole(4, [25, 0, 6])
    geometry.set_pole(5, [15, 15, 6])
    geometry.set_pole(6, [-5, - 5, -3])
    geometry.set_pole(7, [-25, 15, 4])

    return geometry


def test_bspline_dimension(bspline_3d):
    assert_equal(bspline_3d.dimension, 3)


def test_bspline_is_rational(bspline_3d):
    assert_equal(bspline_3d.is_rational, False)


def test_bspline_degree(bspline_3d):
    assert_equal(bspline_3d.degree, 4)


def test_bspline_nb_knots(bspline_3d):
    assert_equal(bspline_3d.nb_knots, 11)


def test_bspline_nb_poles(bspline_3d):
    assert_equal(bspline_3d.nb_poles, 8)


def test_bspline_domain(bspline_3d):
    assert_equal(bspline_3d.domain.t0, 0)
    assert_equal(bspline_3d.domain.t1, 131.892570399495)


def test_bspline_point_at(bspline_3d):
    assert_almost_equal(
        bspline_3d.point_at(t=0.0),
        [0, -25, -5],
    )

    assert_almost_equal(
        bspline_3d.point_at(t=65.9462851997),
        [18.8888888889, -6.1111111111, 4.1666666667],
    )

    assert_almost_equal(
        bspline_3d.point_at(t=131.892570399495),
        [-25, 15, 4],
    )


def test_bspline_derivarives_at(bspline_3d):
    assert_almost_equal(bspline_3d.derivatives_at(t=0.0, order=6), [
        [0, -25, -5],
        [-1.81966277, 1.2131085134, 0.6065542567],
        [0.2759310497, -0.0551862099, -0.0717420729],
        [-0.0189682773, 0.0005578905, 0.005523116],
        [0.0006062836, 0.0000493487, -0.0001894989],
        [0, 0, 0],
        [0, 0, 0],
    ])

    assert_almost_equal(bspline_3d.derivatives_at(t=65.9462851997, order=6), [
        [18.8888888889, -6.1111111111, 4.1666666667],
        [0.2021847522, 0.336974587, 0.1010923761],
        [- 0.0122636022, 0.0153295028, -0.0036790807],
        [-0.0005578905, -0.0006508723, 0.0000557891],
        [-0.0000479387, -0.0000860077, 0.0000236874],
        [0, 0, 0],
        [0, 0, 0],
    ])

    assert_almost_equal(bspline_3d.derivatives_at(t=131.892570399495, order=6), [
        [-25, 15, 4],
        [-2.4262170267, 2.4262170267, 0.8491759593],
        [-0.1103724199, 0.3311172597, 0.1269282829],
        [-0.0044631241, 0.0251050729, 0.0092051934],
        [-0.0001212567, 0.0008417473, 0.0002994759],
        [0, 0, 0],
        [0, 0, 0],
    ])


@pytest.fixture
def nurbs_3d():
    geometry = an.NurbsCurveGeometry3D(
        degree=4,
        nb_poles=8,
        is_rational=True,
    )

    geometry.set_knot(0, 0)
    geometry.set_knot(1, 0)
    geometry.set_knot(2, 0)
    geometry.set_knot(3, 0)
    geometry.set_knot(4, 32.9731425998736)
    geometry.set_knot(5, 65.9462851997473)
    geometry.set_knot(6, 98.9194277996209)
    geometry.set_knot(7, 131.892570399495)
    geometry.set_knot(8, 131.892570399495)
    geometry.set_knot(9, 131.892570399495)
    geometry.set_knot(10, 131.892570399495)

    geometry.set_pole(0, [0, -25, -5])
    geometry.set_pole(1, [-15, -15, 0])
    geometry.set_pole(2, [5, -5, -3])
    geometry.set_pole(3, [15, -15, 3])
    geometry.set_pole(4, [25, 0, 6])
    geometry.set_pole(5, [15, 15, 6])
    geometry.set_pole(6, [-5, -5, -3])
    geometry.set_pole(7, [-25, 15, 4])

    geometry.set_weight(0, 1.0)
    geometry.set_weight(1, 3.0)
    geometry.set_weight(2, 1.0)
    geometry.set_weight(3, 2.5)
    geometry.set_weight(4, 1.0)
    geometry.set_weight(5, 0.5)
    geometry.set_weight(6, 1.0)
    geometry.set_weight(7, 2.0)

    return geometry


def test_nurbs_dimension(nurbs_3d):
    assert_equal(nurbs_3d.dimension, 3)


def test_nurbs_is_rational(nurbs_3d):
    assert_equal(nurbs_3d.is_rational, True)


def test_nurbs_degree(nurbs_3d):
    assert_equal(nurbs_3d.degree, 4)


def test_nurbs_nb_knots(nurbs_3d):
    assert_equal(nurbs_3d.nb_knots, 11)


def test_nurbs_nb_poles(nurbs_3d):
    assert_equal(nurbs_3d.nb_poles, 8)


def test_nurbs_domain(nurbs_3d):
    assert_equal(nurbs_3d.domain.t0, 0)
    assert_equal(nurbs_3d.domain.t1, 131.892570399495)


def test_nurbs_point_at(nurbs_3d):
    assert_almost_equal(
        nurbs_3d.point_at(t=0.0),
        [0, -25, -5],
    )

    assert_almost_equal(
        nurbs_3d.point_at(t=65.9462851997),
        [17.3728814, -10.0847458, 3.6610169],
    )

    assert_almost_equal(
        nurbs_3d.point_at(t=125),
        [-15.8012477, 7.4328264, 1.4566483],
    )

    assert_almost_equal(
        nurbs_3d.point_at(t=131.892570399495),
        [-25, 15, 4],
    )


def test_nurbs_derivatives_at(nurbs_3d):
    assert_almost_equal(nurbs_3d.derivatives_at(t=0.0, order=6), [
        [0., -25., -5.],
        [-5.45898831, 3.63932554, 1.81966277],
        [3.42154502, -2.15226219, -1.12028006],
        [-3.08429768, 1.95373256, 1.01474705],
        [3.72943884, -2.36102637, -1.22629296],
        [-5.63374315, 3.56670634, 1.85254034],
        [10.21284589, -6.46572368, -3.35827683],
    ])

    assert_almost_equal(nurbs_3d.derivatives_at(t=65.9462851997, order=6), [
        [17.37288136, -10.08474576, 3.66101695],
        [0.1575194, 0.21467246, 0.06502903],
        [-0.00117298, 0.01359894, -0.00043955],
        [-2.12457824e-04, -3.09834787e-05, 7.82490021e-05],
        [-2.06752474e-04, -1.09573544e-04, -1.82676037e-05],
        [-2.24469891e-05, -2.74737538e-05, -3.23014355e-06],
        [-3.79282580e-06, -5.32918850e-06, -5.42122028e-07],
    ])

    assert_almost_equal(nurbs_3d.derivatives_at(t=125, order=6), [
        [-15.80124771, 7.43282639, 1.45664835],
        [-1.44436016, 0.92717365, 0.28731735],
        [0.02630252, 0.06594089, 0.03161162],
        [0.0034604, -0.00686368, -0.00341562],
        [-0.0006515, -0.00045743, -0.00014384],
        [1.42132354e-05, 2.51729452e-04, 1.11556135e-04],
        [2.10351592e-05, -2.74544135e-05, -1.47235146e-05],
    ])

    assert_almost_equal(nurbs_3d.derivatives_at(t=131.892570399495, order=6), [
        [-25., 15., 4.],
        [-1.21310851, 1.21310851, 0.42458798],
        [0.03679081, 0.0183954, 0.0091977],
        [-6.59194921e-17, -5.85785035e-03, -2.64997992e-03],
        [-0.00030878, 0.00048644, 0.00024371],
        [5.60168059e-05, 3.24982996e-05, 8.91565193e-06],
        [-3.86134383e-06, -2.21078955e-05, -9.42722295e-06],
    ])


def test_nurbs_greville_point(nurbs_3d):
    assert_almost_equal(
        nurbs_3d.greville_point(index=2),
        24.7298569499,
    )


def test_nurbs_by_lists():
    an.NurbsCurveGeometry3D(
        degree=4,
        knots=[0, 0, 0, 0, 32.9731425998736, 65.9462851997473,
               98.9194277996209, 131.892570399495, 131.892570399495,
               131.892570399495, 131.892570399495],
        poles=[[0, -25, -5], [-15, -15, 0], [5, -5, -3], [15, -15, 3],
               [25, 0, 6], [15, 15, 6], [-5, -5, -3], [-25, 15, 4]],
        weights=[1, 3, 1, 2.5, 1, 0.5, 1, 2],
    )


def test_nurbs_init_with_knots_raises(nurbs_3d):
    with pytest.raises(RuntimeError):
        an.NurbsCurveGeometry3D(
            degree=4,
            knots=np.empty(5),
            poles=np.empty((5, 3)),
        )


def test_nurbs_init_with_weight_raises(nurbs_3d):
    with pytest.raises(RuntimeError):
        an.NurbsCurveGeometry3D(
            degree=4,
            knots=np.empty(8),
            poles=np.empty((5, 3)),
            weights=np.empty(6),
        )
