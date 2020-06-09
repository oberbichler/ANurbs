#pragma once

#include "../Define.h"

#include "IntegrationPoints.h"
#include "TrimmedSurfaceClipping.h"
#include "PolygonIntegrationPoints.h"

#include "../Geometry/BrepFace.h"

namespace anurbs {

class BrepFaceIntegrationPoints
{
public:     // types
    using Vector = Eigen::Matrix<double, 1, 3>;

public:     // static methods
    static IntegrationPointList<2> get(const BrepFace& face, const double tolerance)
    {
        TrimmedSurfaceClipping clipper(tolerance * 10, tolerance / 10);

        IntegrationPointList<2> integration_points;

        for (const auto loop : face.loops()) {
            clipper.begin_loop();

            for (const auto trim : loop->trims()) {
                clipper.add_curve(*trim->curve_2d());
            }

            clipper.end_loop();
        }

        const auto& surface_geometry = *face.surface_geometry().data();
        clipper.compute(surface_geometry.spans_u(), surface_geometry.spans_v());

        const auto degree_u = surface_geometry.degree_u();
        const auto degree_v = surface_geometry.degree_v();

        const auto degree = std::max(degree_u, degree_v) + 1;

        for (Index i = 0; i < clipper.nb_spans_u(); i++) {
            for (Index j = 0; j < clipper.nb_spans_v(); j++) {
                if (clipper.span_trim_type(i, j) == TrimTypes::Empty) {
                    continue;
                }

                if (clipper.span_trim_type(i, j) == TrimTypes::Full) {
                    const auto points = IntegrationPoints::get(degree_u + 1, degree_v + 1, clipper.span_u(i), clipper.span_v(j));

                    for (const auto [u, v, weight] : points) {
                        const auto ders = surface_geometry.derivatives_at(u, v, 1);

                        const auto n = cross(ders[1], ders[2]);

                        integration_points.emplace_back(u, v, weight * norm(n));
                    }
                } else {
                    const auto polygons = clipper.span_polygons(i, j);

                    for (const auto& polygon : polygons) {
                        const auto points = PolygonIntegrationPoints::get(degree, polygon);

                        for (const auto [u, v, weight] : points) {
                            const auto ders = surface_geometry.derivatives_at(u, v, 1);

                            const auto n = cross(ders[1], ders[2]);

                            integration_points.emplace_back(u, v, weight * norm(n));
                        }
                    }
                }
            }
        }

        return integration_points;
    }

    static std::vector<std::tuple<Index, Index, IntegrationPointList<2>>> get_with_spans(const BrepFace& face, const double tolerance)
    {
        TrimmedSurfaceClipping clipper(tolerance * 10, tolerance / 10);

        std::vector<std::tuple<Index, Index, IntegrationPointList<2>>> integration_points;

        for (const auto loop : face.loops()) {
            clipper.begin_loop();

            for (const auto trim : loop->trims()) {
                clipper.add_curve(*trim->curve_2d());
            }

            clipper.end_loop();
        }

        const auto& surface_geometry = *face.surface_geometry().data();
        clipper.compute(surface_geometry.spans_u(), surface_geometry.spans_v());

        const auto degree_u = surface_geometry.degree_u();
        const auto degree_v = surface_geometry.degree_v();

        const auto degree = std::max(degree_u, degree_v) + 1;

        for (Index i = 0; i < clipper.nb_spans_u(); i++) {
            for (Index j = 0; j < clipper.nb_spans_v(); j++) {
                if (clipper.span_trim_type(i, j) == TrimTypes::Empty) {
                    continue;
                }

                const auto [span_u, span_v] = surface_geometry.span_at(clipper.span_u(i).t0(), clipper.span_v(j).t0());
                IntegrationPointList<2> span_integration_points;

                if (clipper.span_trim_type(i, j) == TrimTypes::Full) {
                    const auto points = IntegrationPoints::get(degree_u + 1, degree_v + 1, clipper.span_u(i), clipper.span_v(j));

                    for (const auto [u, v, weight] : points) {
                        const auto ders = surface_geometry.derivatives_at(u, v, 1);

                        const auto n = cross(ders[1], ders[2]);

                        span_integration_points.emplace_back(u, v, weight * norm(n));
                    }
                } else {
                    const auto polygons = clipper.span_polygons(i, j);

                    for (const auto& polygon : polygons) {
                        const auto points = PolygonIntegrationPoints::get(degree, polygon);

                        for (const auto [u, v, weight] : points) {
                            const auto ders = surface_geometry.derivatives_at(u, v, 1);

                            const auto n = cross(ders[1], ders[2]);

                            span_integration_points.emplace_back(u, v, weight * norm(n));
                        }
                    }
                }

                integration_points.push_back({span_u, span_v, span_integration_points});
            }
        }

        return integration_points;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;
        
        using Type = BrepFaceIntegrationPoints;

        m.def("integration_points", [](const BrepFace& face, double tolerance) { return Type::get(face, tolerance); }, "face"_a, "tolerance"_a);
        m.def("integration_points_with_spans", [](const BrepFace& face, double tolerance) { return Type::get_with_spans(face, tolerance); }, "face"_a, "tolerance"_a);
    }
};

} // namespace anurbs