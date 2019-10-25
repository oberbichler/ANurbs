#pragma once

#include "../Define.h"

#include "Interval.h"

#include <vector>

namespace ANurbs {

template <Index TDimension>
class SurfaceBase
{
public:     // types
    using Vector = linear_algebra::Vector<TDimension>;

public:     // constructors
    virtual ~SurfaceBase() { }

public:     // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public:     // methods
    virtual Index degree_u() const = 0;

    virtual Index degree_v() const = 0;

    virtual Interval domain_u() const = 0;

    virtual Interval domain_v() const = 0;

    virtual Vector point_at(const double u, const double v) const = 0;

    virtual std::vector<Vector> derivatives_at(const double u, const double v,
        const Index order) const = 0;

    Vector normal_at(const double u, const double v) const
    {
        static_assert(dimension() == 3);

        const auto a = derivatives_at(u, v, 1);

        const Vector normal = a[1].cross(a[2]);

        return normal;
    }

    virtual std::vector<Interval> spans_u() const = 0;

    virtual std::vector<Interval> spans_v() const = 0;

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = SurfaceBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = "SurfaceBase" + std::to_string(TDimension) +
            "D";

        auto py_class = py::class_<Type, Holder>(m, name.c_str())
            // read-only property
            .def_property_readonly("degree_u", &Type::degree_u)
            .def_property_readonly("degree_v", &Type::degree_v)
            .def_property_readonly("dimension", [](Type&) {
                return Type::dimension(); })
            .def_property_readonly("domain_u", &Type::domain_u)
            .def_property_readonly("domain_v", &Type::domain_v)
            .def_property_readonly("spans_u", &Type::spans_u)
            .def_property_readonly("spans_v", &Type::spans_v)
            // methods
            .def("point_at", &Type::point_at, "u"_a, "v"_a)
            .def("derivatives_at", &Type::derivatives_at, "u"_a, "v"_a,
                "order"_a)
        ;

        if constexpr(dimension() == 3) {
            py_class.def("normal_at", &Type::normal_at, "u"_a, "v"_a);
        }
    }
};

} // namespace ANurbs
