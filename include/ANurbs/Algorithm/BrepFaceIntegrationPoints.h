#pragma once

#include "../Define.h"

#include "IntegrationPoints.h"
#include "TrimmedSurfaceClipping.h"
#include "PolygonIntegrationPoints.h"

#include "../Geometry/BrepFace.h"

namespace ANurbs {

class BrepFaceIntegrationPoints
{
public:     // types
    using Vector = linear_algebra::Vector<3>;

public:     // static methods
    static IntegrationPointList<2> get(const BrepFace& face,
        const double tolerance)
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
                    const auto points = IntegrationPoints::get(degree_u + 1,
                        degree_v + 1, clipper.span_u(i), clipper.span_v(j));

                    for (const auto [u, v, weight] : points) {
                        const auto ders =
                            surface_geometry.derivatives_at(u, v, 1);

                        const auto n = cross(ders[1], ders[2]);

                        integration_points.emplace_back(u, v, weight * norm(n));
                    }
                } else {
                    const auto polygons = clipper.span_polygons(i, j);

                    for (const auto& polygon : polygons) {
                        const auto points =
                            PolygonIntegrationPoints::get(degree, polygon);

                        for (const auto [u, v, weight] : points) {
                            const auto ders =
                                surface_geometry.derivatives_at(u, v, 1);

                            const auto n = cross(ders[1], ders[2]);

                            integration_points.emplace_back(u, v,
                                weight * norm(n));
                        }
                    }
                }
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

        m.def("integration_points", [](const BrepFace& face, double tolerance) {
            return Type::get(face, tolerance); }, "face"_a, "tolerance"_a);
    }
};

} // namespace ANurbs