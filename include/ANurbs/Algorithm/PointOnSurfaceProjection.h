#pragma once

#include "../Define.h"

#include "../Geometry/SurfaceBase.h"

#include <nanoflann/nanoflann.hpp>

namespace ANurbs {

template <Index TDimension>
class PointOnSurfaceProjection
{
public:     // types
    using VectorNd = Vector<TDimension>;
    using SurfaceBase = SurfaceBase<TDimension>;

private:    // types
    struct ParameterPoint
    {
        double parameterU;
        double parameterV;
        VectorNd point;
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

private:    // variables
    Pointer<SurfaceBase> m_surface;
    ParameterPoint m_closestPoint;
    std::vector<ParameterPoint> m_tessellation;
    double m_tolerance;
    double m_distance;
    Index m_grid_u;
    Index m_grid_v;
    Unique<KDTreeType> m_index;
    const PointCloudAdaptor m_point_cloud_adaptor;

public:     // constructors
    PointOnSurfaceProjection(Pointer<SurfaceBase> surface)
        : m_surface(surface), m_point_cloud_adaptor(m_tessellation)
    {
        std::vector<double> us;

        for (const auto& span : surface->spans_u()) {
            if (span.length() < 1e-7) {
                continue;
            }

            const Index n = surface->degree_u() + 1;

            for (Index i = 0; i < n; i++) {
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

            const Index n = surface->degree_v() + 1;

            for (Index i = 0; i < n; i++) {
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

        m_index = new_<KDTreeType>(3, m_point_cloud_adaptor,
            nanoflann::KDTreeSingleIndexAdaptorParams(10));

        m_index->buildIndex();

        m_grid_u = static_cast<Index>(us.size()) - 1;
        m_grid_v = static_cast<Index>(vs.size()) - 1;
    }

    Pointer<SurfaceBase> surface() const
    {
        return m_surface;
    }

    double tolerance() const
    {
        return m_tolerance;
    }

    void set_tolerance(const double value)
    {
        m_tolerance = value;
    }

    double parameter_u() const
    {
        return m_closestPoint.parameterU;
    }

    double parameter_v() const
    {
        return m_closestPoint.parameterV;
    }

    VectorNd point() const
    {
        return m_closestPoint.point;
    }

    double distance() const
    {
        return m_distance;
    }

    void compute(const VectorNd& sample)
    {
        size_t minIndex;
        double minDistance;

        nanoflann::KNNResultSet<double> resultSet(1);
        resultSet.init(&minIndex, &minDistance);
        m_index->findNeighbors(resultSet, &sample[0],
            nanoflann::SearchParams(10));

        // ---

        const size_t p = minIndex % (m_grid_v + 1);
        const size_t o = minIndex / (m_grid_v + 1);

        m_closestPoint = m_tessellation[minIndex];

        if (o != m_grid_u && p != m_grid_v) {
            const auto pt = triangle_projection(sample, minIndex, minIndex + 1, minIndex + m_grid_v + 1);

            const VectorNd v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        if (o != m_grid_u && p != 0) {
            const auto pt = triangle_projection(sample, minIndex, minIndex - 1, minIndex + m_grid_v + 1);

            const VectorNd v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        if (o != 0 && p != m_grid_v) {
            const auto pt = triangle_projection(sample, minIndex, minIndex + 1, minIndex - m_grid_v - 1);

            const VectorNd v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        if (o != 0 && p != 0) {
            const auto pt = triangle_projection(sample, minIndex, minIndex - 1, minIndex - m_grid_v - 1);

            const VectorNd v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < minDistance) {
                m_closestPoint = pt;
                minDistance = distance;
            }
        }

        // ---

        m_closestPoint = newton(sample, m_closestPoint.parameterU, m_closestPoint.parameterV);
    }

    std::vector<double> bounding_box() const
    {
        const Index dimension = TDimension;

        std::vector<double> values(dimension * 2);

        const auto& boundingBox = m_index->root_bbox;

        for (Index axis = 0; axis < dimension; axis++) {
            values[axis] = boundingBox[axis].low;
            values[dimension + axis] = boundingBox[axis].high;
        }

        return values;
    }

    ParameterPoint newton(const VectorNd point, const double u, const double v)
    {
        Vector<2> x(u, v);

        const Index maxiter = 5;
        const double ftol = 1e-8;
        const double gtol = 1e-8;

        for (Index i = 0; i < maxiter; i++) {
            const auto s = m_surface->derivatives_at(x[0], x[1], 2);

            const VectorNd r = point - s[0];
    
            if (r.squaredNorm() < std::pow(ftol, 2)) {
                break;
            }

            const Vector<2> g(-s[1].dot(r), -s[2].dot(r));

            if (g.squaredNorm() < std::pow(gtol, 2)) {
                break;
            }

            const double h_uu = s[1].dot(s[1]) - s[3].dot(r);
            const double h_vv = s[2].dot(s[2]) - s[5].dot(r);
            const double h_uv = s[1].dot(s[2]) - s[4].dot(r);

            const double det = h_uu * h_vv - h_uv * h_uv;

            const double du = (g[1] * h_uv - g[0] * h_vv) / det;
            const double dv = (g[0] * h_uv - g[1] * h_uu) / det;

            x += Vector<2>(du, dv);
        }

        return {x[0], x[1], m_surface->point_at(x[0], x[1])};
    }

    ParameterPoint triangle_projection(const VectorNd point,
        const size_t& index_a, const size_t& index_b, const size_t& index_c)
    {
        const auto a = m_tessellation[index_a];
        const auto b = m_tessellation[index_b];
        const auto c = m_tessellation[index_c];

        const VectorNd u = b.point - a.point;
        const VectorNd v = c.point - a.point;
        const VectorNd n = cross(u, v);
        const VectorNd w = point - a.point;

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
            // constructors
            .def(py::init<Pointer<SurfaceBase>>(), "surface"_a)
            // methods
            .def("compute", &Type::compute, "point"_a)
            // read-only properties
            .def_property_readonly("parameter_u", &Type::parameter_u)
            .def_property_readonly("parameter_v", &Type::parameter_v)
            .def_property_readonly("point", &Type::point)
            .def_property_readonly("distance", &Type::distance)
            .def_property_readonly("bounding_box", &Type::bounding_box)
        ;
    }
};

} // namespace ANurbs
