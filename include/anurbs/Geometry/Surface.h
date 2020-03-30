#pragma once

#include "../Define.h"

#include "Interval.h"
#include "NurbsSurfaceGeometry.h"
#include "SurfaceBase.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

namespace anurbs {

template <Index TDimension, typename TRef=Ref<NurbsSurfaceGeometry<TDimension>>>
class Surface : public SurfaceBase<TDimension>
{
public:     // types
    using Type = Surface<TDimension, TRef>;
    using SurfaceGeometry = NurbsSurfaceGeometry<TDimension>;
    using Vector = typename SurfaceBase<TDimension>::Vector;

private:    // variables
    TRef m_surface_geometry;
    Interval m_domain_u;
    Interval m_domain_v;

public:     // constructors
    Surface(TRef geometry) : Surface(geometry, geometry->domain_u(),
        geometry->domain_v())
    {
    }

    Surface(TRef geometry, const Interval& domain_u, const Interval& domain_v)
        : m_surface_geometry(geometry), m_domain_u(domain_u),
        m_domain_v(domain_v)
    {
    }

public:     // static methods
    using SurfaceBase<TDimension>::dimension;

public:     // methods
    TRef surface_geometry() const
    {
        return m_surface_geometry;
    }

    Index degree_u() const override
    {
        return m_surface_geometry->degree_u();
    }

    Index degree_v() const override
    {
        return m_surface_geometry->degree_v();
    }

    Interval domain_u() const override
    {
        return m_domain_u;
    }

    Interval domain_v() const override
    {
        return m_domain_v;
    }

    Vector point_at(const double u, const double v) const override
    {
        return m_surface_geometry->point_at(u, v);
    }

    std::vector<Vector> derivatives_at(const double u, const double v,
        const Index order) const override
    {
        return m_surface_geometry->derivatives_at(u, v, order);
    }

    std::vector<Interval> spans_u() const override
    {
        auto knots = m_surface_geometry->knots_u();

        Index first_span = Nurbs::upper_span(degree_u(), knots, domain_u().t0());
        Index last_span = Nurbs::lower_span(degree_u(), knots, domain_u().t1());

        Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            double u0 = surface_geometry()->knot_u(first_span + i);
            double u1 = surface_geometry()->knot_u(first_span + i + 1);

            result[i] = Interval(u0, u1);
        }

        return result;
    }

    std::vector<Interval> spans_v() const override
    {
        auto knots = m_surface_geometry->knots_v();

        Index first_span = Nurbs::upper_span(degree_v(), knots, domain_v().t0());
        Index last_span = Nurbs::lower_span(degree_v(), knots, domain_v().t1());

        Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            double v0 = surface_geometry()->knot_v(first_span + i);
            double v1 = surface_geometry()->knot_v(first_span + i + 1);

            result[i] = Interval(v0, v1);
        }

        return result;
    }

public:     // serialization
    static std::string type_name()
    {
        return "Surface" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto geometry = model.get_lazy<NurbsSurfaceGeometry<TDimension>>(
            source.at("geometry"));

        auto result = new_<Type>(geometry);

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["geometry"] = data.surface_geometry().key();
    }

public:     // python
    static std::string python_name()
    {
        return "Surface" + std::to_string(dimension()) + "D";
    }

    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Surface<TDimension, TRef>;
        using Base = SurfaceBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            .def(py::init<TRef>(), "geometry"_a)
            .def("surface_geometry", &Type::surface_geometry)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs
