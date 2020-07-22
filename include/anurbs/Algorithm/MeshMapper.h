#pragma once

#include "../Define.h"

#include "../Geometry/Line.h"
#include "../Geometry/Triangle.h"
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
    using Parameter = Eigen::Matrix<double, 1, 3>;

private: // variables
    Faces m_faces;
    Vertices m_vertices;
    RTree<TDimension> m_rtree;

public: // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public: // constructor
    MeshMapper(Eigen::Ref<const Vertices>& vertices, Eigen::Ref<const Faces>& faces)
        : m_vertices(vertices)
        , m_faces(faces)
        , m_rtree(faces.rows())
    {
        for (Index i = 0; i < faces.rows(); i++) {
            Eigen::Array<double, 3, TDimension> triangle;

            triangle.row(0) = vertices.row(faces(i, 0));
            triangle.row(1) = vertices.row(faces(i, 1));
            triangle.row(2) = vertices.row(faces(i, 2));

            Vector box_a = triangle.colwise().minCoeff();
            Vector box_b = triangle.colwise().maxCoeff();

            m_rtree.add(box_a, box_b);
        }

        m_rtree.finish(true);
    }
    
private: // methods
    bool map_line(const Vector point, const Vector a, const Vector b, double& closest_sq_distance, Vector& closest_point, Parameter& closest_parameter)
    {
        const auto [c_parameter, c_point] = Line<3>::closest_point(point, a, b);

        const double c_sq_distance = (c_point - point).squaredNorm();

        if (c_sq_distance < closest_sq_distance) {
            closest_sq_distance = c_sq_distance;
            closest_point = c_point;
            closest_parameter << c_parameter, 1 - c_parameter, 0;
        
            return true;
        }

        return false;
    }

public: // methods
    std::tuple<bool, Vector, std::pair<Index, Parameter>, double> map(const Vector point, const double max_distance)
    {
        const auto indices = m_rtree.by_point(point, max_distance);

        double closest_sq_distance = Infinity;
        Vector closest_point;
        Index closest_index = -1;
        Parameter closest_parameter = Parameter::Zero();

        for (const Index index : indices) {
            const Vector a = m_vertices.row(m_faces(index, 0));
            const Vector b = m_vertices.row(m_faces(index, 1));
            const Vector c = m_vertices.row(m_faces(index, 2));

            const auto [c_parameter, c_point] = Triangle<TDimension>::closest_point(point, a, b, c);

            bool changed = false;

            if ((c_parameter.array() < 0).any() || (c_parameter.array() > 1).any()) {
                changed |= map_line(point, a, b, closest_sq_distance, closest_point, closest_parameter);
                changed |= map_line(point, b, c, closest_sq_distance, closest_point, closest_parameter);
                changed |= map_line(point, c, a, closest_sq_distance, closest_point, closest_parameter);
            } else {
                const double c_sq_distance = (c_point - point).squaredNorm();

                if (c_sq_distance < closest_sq_distance) {
                    closest_sq_distance = c_sq_distance;
                    closest_point = c_point;
                    closest_parameter << c_parameter;

                    changed = true;
                }
            }

            if (changed) {
                closest_index = index;
            }
        }

        return {(closest_index > -1), closest_point, {closest_index, closest_parameter}, std::sqrt(closest_sq_distance)};
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
            .def(py::init<Eigen::Ref<const Vertices>&, Eigen::Ref<const Faces>&>(), "vertices"_a, "faces"_a)
            // methods
            .def("map", &Type::map, "point"_a, "max_distance"_a);
    }
};

} // namespace anurbs