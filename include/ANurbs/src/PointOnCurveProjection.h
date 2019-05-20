#pragma once

#include "../Geometry/CurveBase.h"
#include "../Geometry/NurbsCurveGeometry.h"
#include "../Geometry/NurbsSurfaceGeometry.h"
#include "../Algorithm/CurveTessellation.h"

#include <utility>

namespace ANurbs {

template <int TDimension>
class PointOnCurveProjection
{
public:     // types
    using CurveBase = CurveBase<TDimension>;
    using Vector = Vector<TDimension>;
    using ParameterPoint = std::pair<double, Vector>;

private:    // variables
    std::vector<std::pair<double, Vector>> m_tessellation;
    Pointer<CurveBase> m_curve;
    double m_tessellationFlatness;
    double m_tolerance;
    double m_parameter;
    Vector m_point;

public:     // constructors
    PointOnCurveProjection(Pointer<CurveBase> curve, const double& tolerance)
        : m_tessellation(), m_curve(curve), m_tessellationFlatness(1e-3),
        m_tolerance(tolerance)
    {
        // create polyline
        m_tessellation = CurveTessellation<TDimension>::compute(*Curve(),
            TessellationFlatness());
    }

public:     // methods
    Pointer<CurveBase> Curve() const
    {
        return m_curve;
    }

    double TessellationFlatness() const
    {
        return m_tessellationFlatness;
    }

    void SetTessellationFlatness(const double value)
    {
        m_tessellationFlatness = value;
    }

    double Tolerance() const
    {
        return m_tolerance;
    }

    void SetTolerance(const double value)
    {
        m_tolerance = value;
    }

    double Parameter() const
    {
        return m_parameter;
    }
    
    Vector Point() const
    {
        return m_point;
    }

    void Compute(const Vector& sample)
    {
        const auto domain = Curve()->domain();

        // closest point to polyline

        double closestParameter;
        Vector closestPoint;

        double closest_sqr_distance = std::numeric_limits<double>::max();

        for (int i = 1; i < m_tessellation.size(); i++) {
            const auto [t0, point0] = m_tessellation[i - 1];
            const auto [t1, point1] = m_tessellation[i];

            const auto [t, point] =
                project_to_line(sample, point0, point1, t0, t1);

            const Vector v = point - sample;

            const double sqr_distance = squared_norm(v);

            if (sqr_distance < closest_sqr_distance) {
                closest_sqr_distance = sqr_distance;
                closestParameter = t;
                closestPoint = point;
            }
        }

        // newton-raphson

        const int max_iter = 5;
        const double eps1 = Tolerance();
        const double eps2 = Tolerance() * 5;

        for (int i = 0; i < max_iter; i++) {
            auto f = Curve()->derivatives_at(closestParameter, 2);

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

            double delta = dot(f[1], dif) / (dot(f[2], dif)
                + squared_norm(f[1]));

            double nextParameter = closestParameter - delta;

            // FIXME: out-of-domain check

            // FIXME: 3. condition: (nextParameter - closestParameter) * f[1].norm();

            closestParameter = domain.clamp(nextParameter);
        }

        closestPoint = Curve()->point_at(closestParameter);
        
        closest_sqr_distance = squared_norm(Vector(sample - closestPoint));

        Vector pointAtT0 = Curve()->point_at(domain.t0());

        if (squared_norm(Vector(sample - pointAtT0)) < closest_sqr_distance) {
            m_parameter = domain.t0();
            m_point = pointAtT0;
            return;
        }

        Vector pointAtT1 = Curve()->point_at(domain.t1());

        if (squared_norm(Vector(sample - pointAtT1)) < closest_sqr_distance) {
            m_parameter = domain.t1();
            m_point = pointAtT1;
            return;
        }

        m_parameter = closestParameter;
        m_point = closestPoint;
    }

private:
    static ParameterPoint
    project_to_line(
        const Vector& point,
        const Vector& a,
        const Vector& b,
        const double& t0,
        const double& t1
    )
    {
        Vector dif = b - a;
        double l = squared_norm(dif);

        if (l < 1e-14) {
            return {t0, a};
        }

        Vector o = a;
        Vector r = dif * (1.0 / l);
        Vector o2pt = point - o;
        double do2ptr = dot(o2pt, r);

        if (do2ptr < 0) {
            return {t0, a};
        }
        
        if (do2ptr > 1) {
            return {t1, b};
        }

        double t = t0 + (t1 - t0) * do2ptr / l;
        Vector closestPoint = o + r * do2ptr;

        return {t, closestPoint};
    }
};

} // namespace ANurbs
