#pragma once

#include "../Define.h"

#include "CurveBase.h"
#include "Interval.h"
#include "NurbsCurveGeometry.h"

#include "../Model/CadAttributes.h"
#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace ANurbs {

template <int TDimension, typename TRef = Pointer<NurbsCurveGeometry<TDimension>>>
struct Curve : public CurveBase<TDimension>
{
public:     // types
    using Type = Curve<TDimension>;
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

public:     // serialization
    using Attributes = CadAttributes;

    static std::string type_name()
    {
        return "Curve" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto geometry = model.GetLazy<NurbsCurveGeometry<TDimension>>(
            source.at("Geometry"));

        const Interval domain = source.at("Domain");

        return new_<Type>(geometry, domain);
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["Geometry"] = data.curve_geometry().Key();
        target["Domain"] = ToJson(data.domain());
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Curve<TDimension>;
        using Holder = Pointer<Type>;
        using Base = CurveBase<TDimension>;

        const std::string name = Type::type_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            .def(py::init<Pointer<CurveGeometry>, Interval>(), "geometry"_a,
                "domain"_a)
            .def("curve_geometry", &Type::curve_geometry)
        ;

        // RegisterDataType<Type>(m, model, name);
    }
};

} // namespace ANurbs