#pragma once

#include "../Define.h"

#include "Interval.h"

namespace anurbs {

template <Index TDimension>
class CurveBase
{
public:     // types
    using Vector = Eigen::Matrix<double, 1, TDimension>;

    virtual ~CurveBase() = default;

public:     // static methods
    static constexpr Index dimension()
    {
        return TDimension;
    }

public:     // methods
    virtual Index degree() const = 0;

    virtual Interval domain() const = 0;

    virtual std::vector<Interval> spans() const = 0;

    virtual Vector point_at(const double t) const = 0;

    virtual std::vector<Vector> derivatives_at(const double t, const Index order)
        const = 0;

public:     // python
    static std::string python_name()
    {
        return "CurveBase" + std::to_string(dimension()) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Holder>(m, name.c_str())
            // read-only properties
            .def_property_readonly("degree", &Type::degree)
            .def_property_readonly("dimension", [](Type&) {
                return Type::dimension(); })
            .def_property_readonly("domain", &Type::domain)
            .def_property_readonly("spans", &Type::spans)
            // methods
            .def("derivatives_at", &Type::derivatives_at, "t"_a, "order"_a)
            .def("point_at", &Type::point_at, "t"_a)
        ;
    }
};

} // namespace anurbs