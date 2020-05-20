#pragma once

#include "../Define.h"

#include "RTree.h"
#include "../Geometry/Line.h"

#include <tuple>
#include <vector>

namespace anurbs {

template <Index TDimension>
class PolylineMapper
{
private:    // types
    using Type = PolylineMapper<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;

private:    // variables
    std::vector<Vector> m_points;
    RTree<TDimension> m_rtree;

public:     // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public:     // constructor
    PolylineMapper(const std::vector<Vector>& points)
    : m_points(points)
    , m_rtree(length(points) - 1)
    {
        for (Index i = 1; i < length(points); i++) {
            Vector box_a = points[i - 1];
            Vector box_b = points[i];

            m_rtree.add(box_a, box_b);
        }

        m_rtree.finish(false);
    }

public:     // methods
    std::tuple<double, Index, double, Index> map(const Vector point, const double tolerance)
    {
        Vector box_a = point;
        Vector box_b = point;
        
        for (Index i = 0; i < TDimension; i++) {
            box_a[i] += tolerance;
            box_b[i] -= tolerance;
        }

        const auto indices = m_rtree.by_box(box_a, box_b);

        double closest_distance = Infinity;
        double closest_parameter = 0;
        Index closest_index = 0;

        for (const Index i : indices) {
            Vector line_a = m_points[i];
            Vector line_b = m_points[i + 1];

            const auto [t, cp] = Line<TDimension>::closest_point(point, line_a, line_b);

            const double distance = (point - cp).norm();

            if (distance > closest_distance || distance > tolerance) {
                continue;
            }
            
            closest_distance = distance;
            closest_parameter = t;
            closest_index = i;
        }

        return {1.0 - closest_parameter, closest_index, closest_parameter, closest_index + 1};
    }

public:     // python
    static std::string python_name()
    {
        return "PolylineMapper" + std::to_string(dimension()) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        const std::string name = Type::python_name();

        py::class_<Type>(m, name.c_str())
            // constructors
            .def(py::init<const std::vector<Vector>&>(), "points"_a)
            // methods
            .def("map", &Type::map, "point"_a, "tolerance"_a)
        ;
    }
};

} // namespace anurbs