#pragma once

#include "../Define.h"

#include "CurveBase.h"
#include "Interval.h"
#include "NurbsCurveGeometry.h"

#include <vector>

namespace ANurbs {

template <int TDimension, typename TRef = Pointer<NurbsCurveGeometry<TDimension>>>
struct Curve : public CurveBase<TDimension>
{
public:     // types
    using CurveGeometry = NurbsCurveGeometry<TDimension>;
    using Vector = typename CurveBase<TDimension>::Vector;

private:    // variables
    TRef m_curve_geometry;
    Interval m_domain;

public:     // constructor
    Curve(TRef curve_geometry, const Interval domain)
        : m_curve_geometry(curve_geometry), m_domain(domain)
    {
        static_assert(TDimension > 0);
    }

public:     // static methods
    using CurveBase<TDimension>::dimension;

public:     // methods
    TRef curve_geometry() const
    {
        return m_curve_geometry;
    }

    int degree() const override
    {
        return m_curve_geometry->degree();
    }

    std::vector<Vector> derivatives_at(const double t, const int order)
        const override
    {
        return m_curve_geometry->derivatives_at(t, order);
    }

    Interval domain() const override
    {
        return m_domain;
    }

    Vector point_at(const double t) const override
    {
        return m_curve_geometry->point_at(t);
    }

    std::vector<Interval> spans() const override
    {
        const auto knots = m_curve_geometry->knots();

        const int first_span = KnotVector::upper_span(degree(), knots,
            domain().t0());
        const int last_span = KnotVector::lower_span(degree(), knots,
            domain().t1());

        const int nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
            double t0 = m_curve_geometry->knot(first_span + i);
            double t1 = m_curve_geometry->knot(first_span + i + 1);

            result[i] = {domain().clamp(t0), domain().clamp(t1)};
        }

        return result;
    }

public:     // python
    template <typename TModule, typename TModel>
    static void register_python(TModule& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Curve<TDimension, Ref<NurbsCurveGeometry<TDimension>>>;
        using Holder = Pointer<Type>;
        using Base = CurveBase<TDimension>;

        const std::string name = "Curve" + std::to_string(dimension()) + "D";

        py::class_<Type, Base, Holder>(m, name.c_str())
            .def(py::init<Ref<CurveGeometry>, Interval>(), "geometry"_a,
                "domain"_a)
            .def("Geometry", &Type::curve_geometry)
        ;

        RegisterDataType<Type>(m, model, name);
    }
};

} // namespace ANurbs