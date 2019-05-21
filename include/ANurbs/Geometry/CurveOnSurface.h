#pragma once

#include "../Define.h"

#include "CurveBase.h"
#include "Interval.h"
#include "NurbsCurveGeometry.h"
#include "NurbsSurfaceGeometry.h"

#include "../Algorithm/CurveSpanIntersection.h"

namespace ANurbs {

template <int TDimension>
class CurveOnSurface : public CurveBase<TDimension>
{
public:     // types
    using CurveGeometry = NurbsCurveGeometry<2>;
    using SurfaceGeometry = NurbsSurfaceGeometry<TDimension>;
    using Vector = typename CurveBase<TDimension>::Vector;

private:    // variables
    Pointer<CurveGeometry> m_curve_geometry;
    Pointer<SurfaceGeometry> m_surface_geometry;
    Interval m_domain;

public:     // constructors
    CurveOnSurface(Pointer<CurveGeometry> curve_geometry,
        Pointer<SurfaceGeometry> surface_geometry, Interval domain)
        : m_curve_geometry(curve_geometry),
        m_surface_geometry(surface_geometry), m_domain(domain)
    {
    }

public:     // static methods
    using CurveBase<TDimension>::dimension;

public:     // methods
    Pointer<CurveGeometry> curve_geometry() const
    {
        return m_curve_geometry;
    }

    Pointer<SurfaceGeometry> surface_geometry() const
    {
        return m_surface_geometry;
    }

    int degree() const override
    {
        return std::max({m_curve_geometry->degree(),
            m_surface_geometry->degree_u(), m_surface_geometry->degree_v()});
    }

    Interval domain() const override
    {
        return m_domain;
    }

    Vector point_at(const double t) const override
    {
        const auto uv = m_curve_geometry->point_at(t);

        const auto point = m_surface_geometry->point_at(uv[0], uv[1]);

        return point;
    }

    std::vector<Vector> derivatives_at(const double t, const int order)
        const override
    {
        // derivatives of base geometries

        const auto curve_derivatives =
            m_curve_geometry->derivatives_at(t, order);

        const double u = curve_derivatives[0][0];
        const double v = curve_derivatives[0][1];

        const auto surface_derivatives =
            m_surface_geometry->derivatives_at(u, v, order);

        // compute combined derivatives

        std::vector<Vector> derivatives(order + 1);

        std::function<Vector(int, int, int)> c;

        c = [&](int order, int i, int j) -> Vector {
            if (order > 0) {
                Vector result = Vector::Zero();

                for (int a = 1; a <= order; a++) {
                    result += (
                        c(order - a, i + 1, j) * curve_derivatives[a][0] +
                        c(order - a, i, j + 1) * curve_derivatives[a][1]
                    ) * Math::binom(order - 1, a - 1);
                }

                return result;
            } else {
                const int index = NurbsSurfaceShapeFunction::shape_index(i, j);
                return surface_derivatives[index];
            }
        };

        for (int i = 0; i <= order; i++) {
            derivatives[i] = c(i, 0, 0);
        }

        return derivatives;
    }

    std::vector<Interval> spans() const override
    {
        Curve<2> curve(curve_geometry(), domain());

        const auto knots_u = surface_geometry()->knots_u();
        const auto knots_v = surface_geometry()->knots_v();

        const auto intersection =
            CurveSpanIntersection::compute(curve, knots_u, knots_v, 1e-4, true);

        const int nb_spans = intersection.size() - 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
            double t0 = intersection[i];
            double t1 = intersection[i + 1];

            result[i] = Interval(t0, t1);
        }

        return result;
    }

public:     // python
    template <typename TModule>
    static void register_python(TModule& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveOnSurface<TDimension>;
        using Base = CurveBase<TDimension>;
        using Handler = Pointer<Type>;

        const std::string name = "CurveOnSurface" + std::to_string(TDimension) +
            "D";

        py::class_<Type, Base, Handler>(m, name.c_str())
            .def(py::init<Pointer<CurveGeometry>, Pointer<SurfaceGeometry>,
                Interval>(), "curve_geometry"_a, "surface_geometry"_a,
                "domain"_a)
        ;
    }
};

} // namespace ANurbs
