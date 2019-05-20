#pragma once

#include "../Define.h"

#include "Interval.h"

namespace ANurbs {

template <int TDimension>
class CurveBase
{
public:     // types
    using Vector = Vector<TDimension>;

    virtual ~CurveBase() = default;

public:     // static methods
    static constexpr int dimension()
    {
        return TDimension;
    }

public:     // methods
    virtual int degree() const = 0;

    virtual Interval domain() const = 0;

    virtual std::vector<Interval> spans() const = 0;

    virtual Vector point_at(const double t) const = 0;

    virtual std::vector<Vector> derivatives_at(const double t, const int order)
        const = 0;

public:     // python
    template <typename TModule>
    static void register_python(TModule& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = "CurveBase" + std::to_string(dimension()) +
            "D";

        py::class_<Type, Holder>(m, name.c_str())
            .def("Degree", &Type::degree)
            .def("Domain", &Type::domain)
            .def("PointAt", &Type::point_at, "t"_a)
            .def("DerivativesAt", &Type::derivatives_at, "t"_a, "order"_a)
            .def("Spans", &Type::spans)
        ;
    }
};

} // namespace ANurbs