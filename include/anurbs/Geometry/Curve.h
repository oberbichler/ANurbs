#pragma once

#include "../Define.h"

#include "CurveBase.h"
#include "Interval.h"
#include "NurbsCurveGeometry.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <vector>

namespace anurbs {

template <Index TDimension,
    typename TRef = Pointer<NurbsCurveGeometry<TDimension>>>
struct Curve : public CurveBase<TDimension>
{
public:     // types
    using Type = Curve<TDimension, TRef>;
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

    Index degree() const override
    {
        return m_curve_geometry->degree();
    }

    std::vector<Vector> derivatives_at(const double t, const Index order)
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

        const Index first_span = Nurbs::upper_span(degree(), knots,
            domain().t0());
        const Index last_span = Nurbs::lower_span(degree(), knots,
            domain().t1());

        const Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            double t0 = m_curve_geometry->knot(first_span + i);
            double t1 = m_curve_geometry->knot(first_span + i + 1);

            result[i] = {domain().clamp(t0), domain().clamp(t1)};
        }

        return result;
    }

public:     // serialization
    static std::string type_name()
    {
        return "Curve" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto geometry = model.get_lazy<NurbsCurveGeometry<TDimension>>(
            source.at("geometry"));

        const Interval domain = source.at("domain");

        return new_<Type>(geometry, domain);
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["geometry"] = data.curve_geometry().key();
        target["domain"] = ToJson(data.domain());
    }

public:     // python
    static std::string python_name()
    {
        return "Curve" + std::to_string(dimension()) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Curve<TDimension, TRef>;
        using Holder = Pointer<Type>;
        using Base = CurveBase<TDimension>;

        const std::string name = Type::python_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            .def(py::init<TRef, Interval>(), "geometry"_a, "domain"_a)
            .def("curve_geometry", &Type::curve_geometry)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs