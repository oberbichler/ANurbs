#pragma once

#include "../Define.h"

#include "IntegrationPoints.h"
#include "PointOnCurveProjection.h"

#include "../Geometry/BrepEdge.h"

namespace anurbs {

class BrepEdgeIntegrationPoints {
public: // types
    using Vector = Eigen::Matrix<double, 1, 3>;

public: // static methods
    static std::pair<IntegrationPointList<1>, IntegrationPointList<1>> get(const BrepEdge& edge, const double tolerance, const double tessellation_tolerance)
    {
        IntegrationPointList<1> integration_points_a;
        IntegrationPointList<1> integration_points_b;

        // FIXME: check nb_trims == 2

        const auto& trim_a = *edge.trim(0);
        const auto& trim_b = *edge.trim(1);

        const auto curve_3d_a = trim_a.curve_3d();
        const auto curve_3d_b = trim_b.curve_3d();

        PointOnCurveProjection<3> projection_a(curve_3d_a, tolerance, tessellation_tolerance);
        PointOnCurveProjection<3> projection_b(curve_3d_b, tolerance, tessellation_tolerance);

        std::vector<double> spans_on_curve_b;

        for (const auto span_b : curve_3d_b->spans()) {
            spans_on_curve_b.emplace_back(span_b.t0());
        }

        for (const auto span_a : curve_3d_a->spans()) {
            const double t_a = span_a.t0();

            const Vector point = curve_3d_a->point_at(t_a);

            projection_b.compute(point);

            const double t_b = projection_b.parameter();

            spans_on_curve_b.emplace_back(t_b);
        }

        spans_on_curve_b.emplace_back(curve_3d_b->domain().t1());

        std::sort(spans_on_curve_b.begin(), spans_on_curve_b.end());

        const auto& face_a = trim_a.face();
        const auto& face_b = trim_b.face();

        const auto& surface_3d_a = face_a->surface_geometry().data();
        const auto& surface_3d_b = face_b->surface_geometry().data();

        const auto degree = std::max({surface_3d_a->degree_u(),
                                surface_3d_a->degree_v(), surface_3d_b->degree_u(),
                                surface_3d_b->degree_v()})
            + 1;

        const auto nb_integration_points = (length(spans_on_curve_b) - 1) * (degree + 1);

        integration_points_a.reserve(nb_integration_points);
        integration_points_b.reserve(nb_integration_points);

        for (Index i = 1; i < length(spans_on_curve_b); i++) {
            const auto t0_b = spans_on_curve_b[i - 1];
            const auto t1_b = spans_on_curve_b[i];

            const Interval span_b(t0_b, t1_b);

            if (span_b.length() < 1e-7) {
                continue;
            }

            const auto points = IntegrationPoints::get(degree, span_b);

            for (const auto& [t_b, weight_b] : points) {
                const auto ders = curve_3d_b->derivatives_at(t_b, 1);

                const auto tangent = ders[1];
                const auto weight = weight_b * norm(tangent);

                integration_points_b.emplace_back(t_b, weight);

                projection_a.compute(ders[0]);

                const auto t_a = projection_a.parameter();

                integration_points_a.emplace_back(t_a, weight);
            }
        }

        return {integration_points_a, integration_points_b};
    }

public: // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = BrepEdgeIntegrationPoints;

        m.def(
            "integration_points", [](const BrepEdge& edge, double tolerance, double tessellation_tolerance) { return Type::get(edge, tolerance, tessellation_tolerance); }, "edge"_a, "tolerance"_a, "tessellation_tolerance"_a=1e-3);
    }
};

} // namespace anurbs