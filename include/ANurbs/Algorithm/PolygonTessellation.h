#pragma once

#include "../Define.h"

#include "TrimmedSurfaceClipping.h"

#include "../Geometry/CurveBase.h"
#include "../Geometry/Polygon.h"

#include <mapbox/earcut.hpp>

#include <vector>

namespace mapbox {
namespace util {

template <typename TVector>
struct nth<0, TVector> {
    static inline double
    get(
        const TVector& vector)
    {
        return vector[0];
    };
};

template <typename TVector>
struct nth<1, TVector> {
    static inline double
    get(
        const TVector& vector)
    {
        return vector[1];
    };
};

} // namespace util
} // namespace mapbox

namespace ANurbs {

class PolygonTessellation
{
public:     // types
    using TriangleIndices = std::tuple<int, int, int>;
    using QuadIndices = std::tuple<int, int, int, int>;

private:    // variables
    std::vector<int> m_triangles;
    std::vector<int> m_quads;

public:     // constructors
    PolygonTessellation()
    {
    }

public:     // methods
    void compute(const Polygon& polygon)
    {
        m_triangles.clear();
        m_quads.clear();

        using Path = std::vector<Vector<2>>;
        using Paths = std::vector<Path>;

        Paths contours;

        contours.push_back(polygon.outer_path);

        for (const auto& inner_path : polygon.inner_paths) {
            contours.push_back(inner_path);
        }

        // FIXME: Check for quad

        m_triangles = mapbox::earcut<int>(contours);
    }

    int nb_triangles() const
    {
        return static_cast<int>(m_triangles.size() / 3);
    }

    TriangleIndices triangle(int index) const
    {
        const int a = m_triangles.at(index * 3 + 0);
        const int b = m_triangles.at(index * 3 + 1);
        const int c = m_triangles.at(index * 3 + 2);

        return {a, b, c};
    }

    int nb_quads() const
    {
        return static_cast<int>(m_quads.size() / 4);
    }

    QuadIndices quad(int index) const
    {
        const int a = m_quads.at(index * 4 + 0);
        const int b = m_quads.at(index * 4 + 1);
        const int c = m_quads.at(index * 4 + 2);
        const int d = m_quads.at(index * 4 + 3);

        return {a, b, c, d};
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = PolygonTessellation;

        std::string name = "PolygonTessellation";

        py::class_<Type>(m, name.c_str())
            .def(py::init<>())
            .def("compute", &Type::compute, "polygon"_a)
            .def("nb_triangles", &Type::nb_triangles)
            .def("triangle", &Type::triangle, "index"_a)
        ;
    }
};

} // namespace ANurbs