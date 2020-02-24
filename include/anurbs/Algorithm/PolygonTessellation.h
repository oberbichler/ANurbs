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

namespace anurbs {

class PolygonTessellation
{
public:     // types
    using TriangleIndices = std::tuple<Index, Index, Index>;
    using QuadIndices = std::tuple<Index, Index, Index, Index>;

private:    // variables
    std::vector<Index> m_triangles;
    std::vector<Index> m_quads;

public:     // constructors
    PolygonTessellation()
    {
    }

public:     // methods
    void compute(const Polygon& polygon)
    {
        m_triangles.clear();
        m_quads.clear();

        using Path = std::vector<linear_algebra::Vector<2>>;
        using Paths = std::vector<Path>;

        Paths contours;

        contours.push_back(polygon.outer_path);

        for (const auto& inner_path : polygon.inner_paths) {
            contours.push_back(inner_path);
        }

        // FIXME: Check for quad

        m_triangles = mapbox::earcut<Index>(contours);
    }

    Index nb_triangles() const
    {
        return static_cast<Index>(m_triangles.size() / 3);
    }

    TriangleIndices triangle(Index index) const
    {
        const Index a = m_triangles.at(index * 3 + 0);
        const Index b = m_triangles.at(index * 3 + 1);
        const Index c = m_triangles.at(index * 3 + 2);

        return {a, b, c};
    }

    Index nb_quads() const
    {
        return static_cast<Index>(m_quads.size() / 4);
    }

    QuadIndices quad(Index index) const
    {
        const Index a = m_quads.at(index * 4 + 0);
        const Index b = m_quads.at(index * 4 + 1);
        const Index c = m_quads.at(index * 4 + 2);
        const Index d = m_quads.at(index * 4 + 3);

        return {a, b, c, d};
    }

public:     // python
    static std::string python_name()
    {
        return "PolygonTessellation";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = PolygonTessellation;

        const std::string name = Type::python_name();

        py::class_<Type>(m, name.c_str())
            .def(py::init<>())
            .def("compute", &Type::compute, "polygon"_a)
            .def("nb_triangles", &Type::nb_triangles)
            .def("triangle", &Type::triangle, "index"_a)
        ;
    }
};

} // namespace anurbs