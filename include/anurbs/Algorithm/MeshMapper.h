#pragma once

#include "../Define.h"

#include "../Geometry/Line.h"
#include "RTree.h"

#include <tuple>
#include <vector>

namespace anurbs {

template <Index TDimension>
class MeshMapper {
private: // types
    using Type = MeshMapper<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;
    using Faces = Eigen::Matrix<Index, Eigen::Dynamic, 3>;
    using Vertices = Eigen::Matrix<double, Eigen::Dynamic, TDimension>;

private: // variables
    Eigen::Matrix<Index, Eigen::Dynamic, 3> m_faces;
    Eigen::Matrix<double, Eigen::Dynamic, TDimension> m_vertices;
    RTree<TDimension> m_rtree;

public: // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public: // constructor
    MeshMapper(Eigen::Ref<const Faces>& faces, Eigen::Ref<const Vertices>& vertices)
        : m_faces(faces)
        , m_vertices(vertices)
        , m_rtree(length(faces))
    {
        for (Index i = 0; i < length(faces); i++) {
            Eigen::Array<double, 3, TDimension> triangle;

            triangle.row(0) = vertices.row(faces(i, 0));
            triangle.row(1) = vertices.row(faces(i, 1));
            triangle.row(2) = vertices.row(faces(i, 2));

            Vector box_a = triangle.colwise().minCoeff();
            Vector box_b = triangle.colwise().maxCoeff();

            m_rtree.add(box_a, box_b);
        }

        m_rtree.finish(false);
    }

public: // methods
    std::tuple<Index, Eigen::Matrix<double, 1, 3>> map(const Vector point, const double max_distance)
    {
        Vector box_a = point;
        Vector box_b = point;

        for (Index i = 0; i < TDimension; i++) {
            box_a[i] += max_distance;
            box_b[i] -= max_distance;
        }

        const auto indices = m_rtree.by_box(box_a, box_b);

        double closest_distance = Infinity;
        Eigen::Matrix<double, 1, 3> closest_parameter = Eigen::Matrix<double, 1, 3>::Zero();
        Index closest_index = -1;

        for (const Index i : indices) {
            Vector a = m_vertices.row(m_faces(i, 0));
            Vector b = m_vertices.row(m_faces(i, 1));
            Vector c = m_vertices.row(m_faces(i, 2));

            const auto [rst, cp] = Triangle<TDimension>::closest_point(point, a, b, c);

            const double distance = (point - cp).norm();

            if (distance > closest_distance || distance > max_distance) {
                continue;
            }

            closest_distance = distance;
            closest_parameter = rst;
            closest_index = i;
        }

        return {closest_index, closest_parameter};
    }

public: // python
    static std::string python_name()
    {
        return "MeshMapper" + std::to_string(dimension()) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        const std::string name = Type::python_name();

        py::class_<Type>(m, name.c_str())
            // constructors
            .def(py::init<Eigen::Ref<const Faces>&, Eigen::Ref<const Vertices>&>(), "faces"_a, "vertices"_a)
            // methods
            .def("map", &Type::map, "point"_a, "max_distance"_a);
    }
};

} // namespace anurbs