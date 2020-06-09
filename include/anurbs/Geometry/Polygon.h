#pragma once

#include "../Define.h"

#include "../Model/Attributes.h"

#include <vector>

namespace anurbs {

struct Polygon
{
public:     // types
    using Vector = Eigen::Matrix<double, 1, 2>;
    using Path = std::vector<Vector>;

public:     // variables
    Path outer_path;
    std::vector<Path> inner_paths;

public:     // methods
    Index nb_loops() const
    {
        return static_cast<Index>(inner_paths.size()) + 1;
    }

    Index nb_vertices_of_loop(const Index index) const
    {
        if (index == 0) {
            return static_cast<Index>(outer_path.size());
        } else {
            return static_cast<Index>(inner_paths[index - 1].size());
        }
    }

    Vector vertex_of_loop(Index loopIndex, Index vertexIndex) const
    {
        if (loopIndex == 0) {
            return outer_path[vertexIndex];
        } else {
            return inner_paths[loopIndex - 1][vertexIndex];
        }
    }

    Index nb_vertices() const
    {
        Index nbVertices = static_cast<Index>(outer_path.size());

        for (const auto& path : inner_paths) {
            nbVertices += static_cast<Index>(path.size());
        }

        return nbVertices;
    }

    Vector vertex(Index index) const
    {
        if (index < length(outer_path)) {
            return outer_path[index];
        }

        Index offset = length(outer_path);

        for (const auto& path : inner_paths) {
            if (index < offset + length(path)) {
                return path[index - offset];
            }

            offset += length(path);
        }

        throw std::exception();
    }

public:     // python
    static std::string python_name()
    {
        return "Polygon2D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Polygon;

        const std::string name = Type::python_name();

        pybind11::class_<Type>(m, name.c_str())
            .def(py::init<>())
            .def("nb_loops", &Type::nb_loops)
            .def("nb_vertices_of_loop", &Type::nb_vertices_of_loop, "index"_a)
            .def("vertex_of_loop", &Type::vertex_of_loop, "loop_index"_a,
                "vertex_index"_a)
            .def("nb_vertices", &Type::nb_vertices)
            .def("vertex", &Type::vertex, "index"_a)
        ;
    }
};

} // namespace anurbs
