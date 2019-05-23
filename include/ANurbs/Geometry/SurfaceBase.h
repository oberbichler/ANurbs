#pragma once

#include "../Define.h"

#include "Interval.h"

#include <vector>

namespace ANurbs {

template <int TDimension>
class SurfaceBase
{
public:     // types
    using Vector = Vector<TDimension>;

public:     // constructors
    virtual ~SurfaceBase() { }

public:     // static methods
    static constexpr int dimension()
    {
        return TDimension;
    }

public:     // methods
    virtual int degree_u() const = 0;

    virtual int degree_v() const = 0;

    virtual Interval domain_u() const = 0;

    virtual Interval domain_v() const = 0;

    virtual Vector point_at(const double u, const double v) const = 0;

    virtual std::vector<Vector> derivatives_at(const double u, const double v,
        const int order) const = 0;

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

        py::class_<Type, Holder>(m, name.c_str())
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
    }
};

} // namespace ANurbs
