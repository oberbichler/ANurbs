#pragma once

#include "../Define.h"

#include "../Model/Attributes.h"

#include <vector>

namespace anurbs {

template<Index TDimension>
struct Triangle
{
public:     // types
    using Vector = linear_algebra::Vector<TDimension>;
    using Vector3 = linear_algebra::Vector<3>;

public:     // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

    static std::pair<Vector, Vector3> projection(Vector point, Vector a, Vector b, Vector c)
    {
        Vector u = b - a;
        Vector v = c - a;
        Vector n = u.cross(v);
        Vector w = point - a;

        double r = u.cross(w).dot(n) / n.dot(n);
        double s = w.cross(v).dot(n) / n.dot(n);
        double t = 1.0 - r - s;

        Vector closest_point = t * a + s * b + r * c;

        return {closest_point, Vector3(t, s, r)};
    }

public:     // python
    static std::string python_name()
    {
        return "Triangle" + std::to_string(dimension()) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Triangle<TDimension>;

        const std::string name = Type::python_name();

        pybind11::class_<Type>(m, name.c_str())
            .def_static("projection", &Type::projection, "point"_a, "a"_a, "b"_a, "c"_a)
        ;
    }
};

} // namespace anurbs
