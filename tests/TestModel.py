import anurbs as an

import os
import pytest

from numpy.testing import assert_equal

if __name__ == '__main__':
    import sys
    pytest.main(sys.argv)


def path(relative_path):
    return os.path.join(os.path.dirname(__file__), relative_path)


@pytest.fixture
def data():
    return r"""[
        {
            "key": "TestData",
            "type": "NurbsCurveGeometry2D",
            "degree": 1,
            "nb_poles": 2,
            "knots": [0, 1],
            "poles": [[1, 2], [3, 4]]
        }
    ]"""


@pytest.fixture
def curve():
    return an.NurbsCurveGeometry2D(degree=1, knots=[0, 1], poles=[[1, 2], [3, 4]])


def test_load():
    model = an.Model()

    model.load(path('data/model.ibra'))

    key, data = model.get(0)

    assert_equal(key, 'TestData')
    assert_equal(type(data), an.NurbsCurveGeometry2D)


def test_load_throws_if_file_not_exists():
    model = an.Model()

    with pytest.raises(RuntimeError) as ex:
        model.load('invalid_filename')

    assert_equal('File not found', str(ex.value))


def test_load_array(data):
    model = an.Model()

    model.add_array(data)

    key, data = model.get(0)

    assert_equal(key, 'TestData')
    assert_equal(type(data), an.NurbsCurveGeometry2D)


def test_to_string(curve):
    model = an.Model()

    model.add(key='TestData', data=curve)

    actual = model.to_string()
    expected = r"""[
                    {
                    "key": "TestData",
                    "type": "NurbsCurveGeometry2D",
                    "degree": 1,
                    "knots": [0.0,1.0],
                    "nb_poles": 2,
                    "poles": [[1.0,2.0],[3.0,4.0]]
                    }
                ]"""

    for actual_line, expected_line in zip(actual.split('\n'), expected.split('\n')):
        assert_equal(actual_line.strip(), expected_line.strip())
