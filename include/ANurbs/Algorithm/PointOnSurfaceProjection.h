#pragma once

#include "../Define.h"

#include "../Geometry/SurfaceBase.h"

#include <nanoflann/nanoflann.hpp>

namespace ANurbs {

template <int TDimension>
class PointOnSurfaceProjection
{
public:
    using Vector = Vector<TDimension>;
    using SurfaceBase = SurfaceBase<TDimension>;

private:
    struct ParameterPoint
    {
        double parameterU;
        double parameterV;
        Vector point;
    };

    struct PointCloudAdaptor
    {
        const std::vector<ParameterPoint>& m_points;

        PointCloudAdaptor(
            const std::vector<ParameterPoint>& points)
        : m_points(points)
        { }

        inline size_t
        kdtree_get_point_count() const
        { 
            return m_points.size();
        }

        inline double
        kdtree_get_pt(
            const size_t idx,
            const size_t dim) const
        {
            return m_points[idx].point[dim];
        }

        template <typename BBOX>
        bool kdtree_get_bbox(
            BBOX&) const
        { 
            return false;
        }
    };

    using KDTreeType = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<double, PointCloudAdaptor>,
        PointCloudAdaptor, 3>;

private:
    Pointer<SurfaceBase> m_surface;
    ParameterPoint m_closestPoint;
    std::vector<ParameterPoint> m_tessellation;
    double m_tolerance;
    double m_distance;
    int m_gridU;
    int m_gridV;
    Unique<KDTreeType> m_index;
    const PointCloudAdaptor m_pointCloudAdaptor;

public:
    PointOnSurfaceProjection(
        Pointer<SurfaceBase> surface)
    : m_surface(surface)
    , m_pointCloudAdaptor(m_tessellation)
    {
        std::vector<double> us;

        for (const auto& span : surface->spans_u()) {
            if (span.length() < 1e-7) {
                continue;
            }

            const int n = surface->degree_u() + 1;

            for (int i = 0; i < n; i++) {
                const double u = span.parameter_at_normalized(1.0 / n * i);
                us.push_back(u);
            }
        }

        us.push_back(surface->domain_u().t1());

        std::vector<double> vs;

        for (const auto& span : surface->spans_v()) {
            if (span.length() < 1e-7) {
                continue;
            }

            const int n = surface->degree_v() + 1;

            for (int i = 0; i < n; i++) {
                const double v = span.parameter_at_normalized(1.0 / n * i);
                vs.push_back(v);
            }
        }

        vs.push_back(surface->domain_v().t1());

        m_tessellation.reserve(us.size() * vs.size());

        for (const auto u : us) {
            for (const auto v : vs) {
                const auto point = m_surface->point_at(u, v);
                m_tessellation.push_back({u, v, point});
            }
        }

        m_index = new_<KDTreeType>(3, m_pointCloudAdaptor,
            nanoflann::KDTreeSingleIndexAdaptorParams(10));

        m_index->buildIndex();

        m_gridU = static_cast<int>(us.size()) - 1;
        m_gridV = static_cast<int>(vs.size()) - 1;
    }

    Pointer<SurfaceBase>
    Surface() const
    {
        return m_surface;
    }

    double
    Tolerance() const
    {
        return m_tolerance;
    }

    void
    SetTolerance(
        double value)
    {
        m_tolerance = value;
    }

    double
    parameter_u() const
    {
        return m_closestPoint.parameterU;
    }

    double
    parameter_v() const
    {
        return m_closestPoint.parameterV;
    }

    Vector
    point() const
    {
        return m_closestPoint.point;
    }

    double
    distance() const
    {
        return m_distance;
    }

    void
    compute(
        const Vector& sample)
    {
        size_t minIndex;
        double minDistance;

        nanoflann::KNNResultSet<double> resultSet(1);
        resultSet.init(&minIndex, &minDistance);
        m_index->findNeighbors(resultSet, &sample[0],
            nanoflann::SearchParams(10));

        // ---

        const size_t p = minIndex % (m_gridV + 1);
        const size_t o = minIndex / (m_gridV + 1);

        m_closestPoint = m_tessellation[minIndex];

        if (o != m_gridU && p != m_gridV) {
            const auto pt = TriangleProjection(sample, minIndex, minIndex + 1, minIndex + m_gridV + 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        if (o != m_gridU && p != 0) {
            const auto pt = TriangleProjection(sample, minIndex, minIndex - 1, minIndex + m_gridV + 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        if (o != 0 && p != m_gridV) {
            const auto pt = TriangleProjection(sample, minIndex, minIndex + 1, minIndex - m_gridV - 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        if (o != 0 && p != 0) {
            const auto pt = TriangleProjection(sample, minIndex, minIndex - 1, minIndex - m_gridV - 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        // ---

        m_closestPoint = Newton(sample, m_closestPoint.parameterU, m_closestPoint.parameterV);
    }

    std::vector<double>
    bounding_box() const
    {
        const int dimension = TDimension;

        std::vector<double> values(dimension * 2);

        const auto& boundingBox = m_index->root_bbox;

        for (int axis = 0; axis < dimension; axis++) {
            values[axis] = boundingBox[axis].low;
            values[dimension + axis] = boundingBox[axis].high;
        }

        return values;
    }

    ParameterPoint
    Newton(
        const Vector point,
        const double u,
        const double v
    )
    {
        double cu = u;
        double cv = v;

        int maxits = 5;
        double eps1 = 0.00001;
        double eps2 = 0.000005;
        
        double minu = m_surface->domain_u().t0();
        double maxu = m_surface->domain_u().t1();
        double minv = m_surface->domain_v().t0();
        double maxv = m_surface->domain_v().t1();

        for (int i = 0; i < maxits; i++) {
            const auto s = m_surface->derivatives_at(cu, cv, 2);

            const Vector dif = s[0] - point;

            const double c1v = norm(dif);
            
            if (c1v < eps1) {
                return {cu, cv, s[0]};
            }

            double s1_l = norm(s[1]);
            double s2_l = norm(s[2]);
            
            double c2an = std::abs(dot(s[1], dif));
            double c2ad = s1_l * c1v;

            double c2bn = std::abs(dot(s[2], dif));
            double c2bd = s2_l * c1v;

            double c2av = c2an / c2ad;
            double c2bv = c2bn / c2bd;

            if (c2av < eps2 && c2bv < eps2) {
                return {cu, cv, s[0]};
            }
            
            double f = dot(s[1], dif);
            double g = dot(s[2], dif);

            double J00 = dot(s[1], s[1]) + dot(s[3], dif);
            double J01 = dot(s[1], s[2]) + dot(s[4], dif);
            double J11 = dot(s[2], s[2]) + dot(s[5], dif);

            double det = J00 * J11 - J01 * J01;
            
            double du = (g * J01 - f * J11) / det;
            double dv = (f * J01 - g * J00) / det;
            
            cu += du;
            cv += dv;
        }

        return {cu, cv, m_surface->point_at(cu, cv)};
    }

    ParameterPoint
    TriangleProjection(
        const Vector point,
        const size_t& indexA,
        const size_t& indexB,
        const size_t& indexC)
    {
        const auto a = m_tessellation[indexA];
        const auto b = m_tessellation[indexB];
        const auto c = m_tessellation[indexC];

        const Vector u = b.point - a.point;
        const Vector v = c.point - a.point;
        const Vector n = cross(u, v);
        const Vector w = point - a.point;

        const double gam = dot(cross(u, w), n) / squared_norm(n);
        const double bet = dot(cross(w, v), n) / squared_norm(n);
        const double alp = 1.0 - gam - bet;

        ParameterPoint cp;

        cp.parameterU = alp * a.parameterU + bet * b.parameterU +
            gam * c.parameterU;
        cp.parameterV = alp * a.parameterV + bet * b.parameterV +
            gam * c.parameterV;

        cp.point = m_surface->point_at(cp.parameterU, cp.parameterV);

        return cp;
    }

public:     // python

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = PointOnSurfaceProjection<TDimension>;
        using Handler = Pointer<Type>;

        std::string name = "PointOnSurfaceProjection" +
            std::to_string(TDimension) + "D";

        py::class_<Type, Handler>(m, name.c_str())
            .def(py::init<Pointer<SurfaceBase>>(), "surface"_a)
            .def("compute", &Type::compute, "point"_a)
            .def("parameter_u", &Type::parameter_u)
            .def("parameter_v", &Type::parameter_v)
            .def("point", &Type::point)
            .def("distance", &Type::distance)
            .def("bounding_box", &Type::bounding_box)
        ;
    }
};

} // namespace ANurbs
