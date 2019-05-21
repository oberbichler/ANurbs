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
            .def("DegreeU", &Type::degree_u)
            .def("DegreeV", &Type::degree_v)
            .def("DomainU", &Type::domain_u)
            .def("DomainV", &Type::domain_v)
            .def("PointAt", &Type::point_at, "u"_a, "v"_a)
            .def("DerivativesAt", &Type::derivatives_at, "u"_a, "v"_a, "order"_a)
            .def("SpansU", &Type::spans_u)
            .def("SpansV", &Type::spans_v)
        ;
    }
};

} // namespace ANurbs
