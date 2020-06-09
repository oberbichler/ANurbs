#pragma once

#include "../Define.h"

#include "CurveTessellation.h"
#include "PolylineMapper.h"

#include "../Geometry/CurveBase.h"
#include "../Geometry/NurbsCurveGeometry.h"
#include "../Geometry/NurbsSurfaceGeometry.h"

#include <utility>

namespace anurbs {

template <Index TDimension>
class PointOnCurveProjection {
public: // types
    using CurveBaseD = CurveBase<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;
    using ParameterPoint = std::pair<double, Vector>;

private: // variables
    std::pair<std::vector<double>, std::vector<Vector>> m_tessellation;
    Pointer<CurveBaseD> m_curve;
    double m_tessellation_tolerance;
    double m_tolerance;
    double m_parameter;
    Vector m_point;
    Unique<PolylineMapper<TDimension>> mapper;

public: // constructors
    PointOnCurveProjection(Pointer<CurveBaseD> curve, const double tolerance, const double tessellation_tolerance)
        : m_tessellation()
        , m_curve(curve)
        , m_tessellation_tolerance(tessellation_tolerance)
        , m_tolerance(tolerance)
    {
        // new_ polyline
        m_tessellation = CurveTessellation<TDimension>::compute(*Curve(), m_tessellation_tolerance);
        mapper = new_<PolylineMapper<TDimension>>(m_tessellation.second);
    }

public: // methods
    Pointer<CurveBaseD> Curve() const
    {
        return m_curve;
    }

    double tessellation_tolerance() const
    {
        return m_tessellation_tolerance;
    }

    double tolerance() const
    {
        return m_tolerance;
    }

    void set_tolerance(const double value)
    {
        m_tolerance = value;
    }

    double parameter() const
    {
        return m_parameter;
    }

    Vector point() const
    {
        return m_point;
    }

    void compute(const Vector& sample, const double max_distance = 0)
    {
        const auto domain = Curve()->domain();

        // closest point to polyline

        double closest_parameter = 0;
        Vector closest_point;

        double closest_sq_distance = Infinity;

        const auto& [ts, points] = m_tessellation;

        if (max_distance <= 0) {
            for (Index i = 1; i < length(ts); i++) {
                const auto t0 = ts[i - 1];
                const auto point0 = points[i - 1];
                const auto t1 = ts[i];
                const auto point1 = points[i];

                const auto [t, point] = project_to_line(sample, point0, point1, t0, t1);

                const Vector v = point - sample;

                const double sq_distance = squared_norm(v);

                if (sq_distance < closest_sq_distance) {
                    closest_sq_distance = sq_distance;
                    closest_parameter = t;
                    closest_point = point;
                }
            }
        } else {
            const auto [wa, idx_a, wb, idx_b] = mapper->map(sample, max_distance);

            closest_parameter = wa * ts[idx_a] + wb * ts[idx_b];
            closest_point = wa * points[idx_a] + wb * points[idx_b];
        }

        // newton-raphson

        const Index max_iter = 5;
        const double eps1 = tolerance();
        const double eps2 = tolerance() * 5;

        for (Index i = 0; i < max_iter; i++) {
            auto f = Curve()->derivatives_at(closest_parameter, 2);

            Vector dif = f[0] - sample;

            double c1v = norm(dif);

            double c2n = dot(f[1], dif);
            double c2d = norm(f[1]) * c1v;
            double c2v = c2d != 0 ? c2n / c2d : 0;

            bool c1 = c1v < eps1;
            bool c2 = std::abs(c2v) < eps2;

            if (c1 || c2) { // FIXME: check if 'or' is correct (NURBS Book P.231)
                break;
            }

            double delta = dot(f[1], dif) / (dot(f[2], dif) + squared_norm(f[1]));

            double nextParameter = closest_parameter - delta;

            // FIXME: out-of-domain check

            // FIXME: 3. condition: (nextParameter - closest_parameter) * f[1].norm();

            closest_parameter = domain.clamp(nextParameter);
        }

        closest_point = Curve()->point_at(closest_parameter);

        closest_sq_distance = squared_norm(Vector(sample - closest_point));

        Vector point_at_t0 = Curve()->point_at(domain.t0());

        if (squared_norm(Vector(sample - point_at_t0)) < closest_sq_distance) {
            m_parameter = domain.t0();
            m_point = point_at_t0;
            return;
        }

        Vector point_at_t1 = Curve()->point_at(domain.t1());

        if (squared_norm(Vector(sample - point_at_t1)) < closest_sq_distance) {
            m_parameter = domain.t1();
            m_point = point_at_t1;
            return;
        }

        m_parameter = closest_parameter;
        m_point = closest_point;
    }

private: // static methods
    static ParameterPoint project_to_line(const Vector& point, const Vector& a, const Vector& b, const double& t0, const double& t1)
    {
        const Vector dif = b - a;
        const double l = squared_norm(dif);

        if (l < 1e-14) {
            return {t0, a};
        }

        const Vector o = a;
        const Vector r = dif * (1.0 / l);
        const Vector o2pt = point - o;
        const double do2ptr = dot(o2pt, r);

        if (do2ptr < 0) {
            return {t0, a};
        }

        if (do2ptr > 1) {
            return {t1, b};
        }

        const double t = t0 + (t1 - t0) * do2ptr;
        const Vector closest_point = o + dif * do2ptr;

        return {t, closest_point};
    }

public: // python
    static std::string python_name()
    {
        return "PointOnCurveProjection" + std::to_string(TDimension) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = PointOnCurveProjection<TDimension>;
        using Handler = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Handler>(m, name.c_str())
            .def(py::init<Pointer<CurveBaseD>, double, double>(), "curve"_a, "tolerance"_a, "tessellation_tolerance"_a = 1e-3)
            .def("compute", &Type::compute, "point"_a, "max_distance"_a = 0)
            .def_property_readonly("parameter", &Type::parameter)
            .def_property_readonly("point", &Type::point);
    }
};

} // namespace anurbs
