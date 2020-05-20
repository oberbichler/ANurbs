#pragma once

#include "../Define.h"

#include "../Geometry/SurfaceBase.h"

#include <nanoflann/nanoflann.hpp>

namespace anurbs {

template <Index TDimension>
class PointOnSurfaceProjection
{
public:     // types
    using Vector = Eigen::Matrix<double, 1, TDimension>;
    using SurfaceBaseD = SurfaceBase<TDimension>;
    using ProjectionResult = std::tuple<bool, double, double, Vector>;

private:    // types
    struct ParameterPoint
    {
        double parameter_u;
        double parameter_v;
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

private:    // variables
    Pointer<SurfaceBaseD> m_surface;
    std::vector<ParameterPoint> m_tessellation;
    double m_tolerance;
    Index m_grid_u;
    Index m_grid_v;
    Unique<KDTreeType> m_index;
    const PointCloudAdaptor m_point_cloud_adaptor;

public:     // constructors
    PointOnSurfaceProjection(Pointer<SurfaceBaseD> surface)
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

        m_tessellation.reserve(length(us) * length(vs));

        for (const auto u : us) {
            for (const auto v : vs) {
                const auto point = m_surface->point_at(u, v);
                m_tessellation.push_back({u, v, point});
            }
        }

        m_index = new_<KDTreeType>(3, m_point_cloud_adaptor,
            nanoflann::KDTreeSingleIndexAdaptorParams(10));

        m_index->buildIndex();

        m_grid_u = length(us) - 1;
        m_grid_v = length(vs) - 1;
    }

    Pointer<SurfaceBaseD> surface() const
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

    ProjectionResult get(const Vector& sample)
    {
        size_t min_index;
        double min_distance;

        nanoflann::KNNResultSet<double> resultSet(1);
        resultSet.init(&min_index, &min_distance);
        m_index->findNeighbors(resultSet, &sample[0],
            nanoflann::SearchParams(10));

        // ---

        const size_t p = min_index % (m_grid_v + 1);
        const size_t o = min_index / (m_grid_v + 1);

        ParameterPoint closest_point = m_tessellation[min_index];

        if (o != m_grid_u && p != m_grid_v) {
            const auto pt = triangle_projection(sample, min_index, min_index + 1, min_index + m_grid_v + 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < min_distance) {
                closest_point = pt;
                min_distance = distance;
            }
        }

        if (o != m_grid_u && p != 0) {
            const auto pt = triangle_projection(sample, min_index, min_index - 1, min_index + m_grid_v + 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < min_distance) {
                closest_point = pt;
                min_distance = distance;
            }
        }

        if (o != 0 && p != m_grid_v) {
            const auto pt = triangle_projection(sample, min_index, min_index + 1, min_index - m_grid_v - 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < min_distance) {
                closest_point = pt;
                min_distance = distance;
            }
        }

        if (o != 0 && p != 0) {
            const auto pt = triangle_projection(sample, min_index, min_index - 1, min_index - m_grid_v - 1);

            const Vector v = sample - pt.point;
            const auto distance = squared_norm(v);

            if (distance < min_distance) {
                closest_point = pt;
                min_distance = distance;
            }
        }

        // ---

        return newton(sample, closest_point.parameter_u, closest_point.parameter_v);
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

    ProjectionResult newton(const Vector point, const double u, const double v)
    {
        using Vector2d = Eigen::Matrix<double, 1, 2>;

        Vector2d x(u, v);

        const Index maxiter = 5;
        const double ftol = 1e-8;
        const double gtol = 1e-8;
        bool success = false;

        std::vector<Vector> s;

        for (Index i = 0; i < maxiter; i++) {
            s = m_surface->derivatives_at(x[0], x[1], 2);

            const Vector r = point - s[0];
    
            if (r.squaredNorm() < std::pow(ftol, 2)) {
                success = true;
                break;
            }

            const Vector2d g(-s[1].dot(r), -s[2].dot(r));

            if (g.squaredNorm() < std::pow(gtol, 2)) {
                success = true;
                break;
            }

            const double h_uu = s[1].dot(s[1]) - s[3].dot(r);
            const double h_vv = s[2].dot(s[2]) - s[5].dot(r);
            const double h_uv = s[1].dot(s[2]) - s[4].dot(r);

            const double det = h_uu * h_vv - h_uv * h_uv;

            const double du = (g[1] * h_uv - g[0] * h_vv) / det;
            const double dv = (g[0] * h_uv - g[1] * h_uu) / det;

            x += Vector2d(du, dv);
        }

        return {success, x[0], x[1], s[0]};
    }

    ParameterPoint triangle_projection(const Vector point,
        const size_t& index_a, const size_t& index_b, const size_t& index_c)
    {
        const auto a = m_tessellation[index_a];
        const auto b = m_tessellation[index_b];
        const auto c = m_tessellation[index_c];

        const Vector u = b.point - a.point;
        const Vector v = c.point - a.point;
        const Vector n = cross(u, v);
        const Vector w = point - a.point;

        const double gam = dot(cross(u, w), n) / squared_norm(n);
        const double bet = dot(cross(w, v), n) / squared_norm(n);
        const double alp = 1.0 - gam - bet;

        const double parameter_u = alp * a.parameter_u + bet * b.parameter_u +
            gam * c.parameter_u;
        const double parameter_v = alp * a.parameter_v + bet * b.parameter_v +
            gam * c.parameter_v;

        const Vector closest_point =
            m_surface->point_at(parameter_u, parameter_v);

        return {parameter_u, parameter_v, closest_point};
    }

public:     // python
    static std::string python_name()
    {
        return "PointOnSurfaceProjection" + std::to_string(TDimension) + "D";
    }

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = PointOnSurfaceProjection<TDimension>;
        using Handler = Pointer<Type>;

        const std::string name = Type::python_name();

        py::class_<Type, Handler>(m, name.c_str())
            // constructors
            .def(py::init<Pointer<SurfaceBaseD>>(), "surface"_a)
            // methods
            .def("get", &Type::get, "point"_a)
            // read-only properties
            .def_property_readonly("bounding_box", &Type::bounding_box)
        ;
    }
};

} // namespace anurbs
