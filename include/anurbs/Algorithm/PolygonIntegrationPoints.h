#pragma once

#include "../Define.h"

#include "IntegrationPoints.h"
#include "PolygonTessellation.h"

#include "../Geometry/Interval.h"
#include "../Geometry/Polygon.h"

#include <stdexcept>
#include <vector>

namespace anurbs {

class PolygonIntegrationPoints
{
public:     // static methods
    static IntegrationPointList<2> get(const Index degree, const Polygon& polygon)
    {
        using Vector2d = Eigen::Matrix<double, 1, 2>;

        IntegrationPointList<2> integration_points;
        PolygonTessellation tessellation;

        tessellation.compute(polygon);

        const auto& xiao_gimbutas = IntegrationPoints::xiao_gimbutas(degree);

        const Index nb_integration_points = tessellation.nb_triangles() *
            static_cast<Index>(xiao_gimbutas.size());

        integration_points.resize(nb_integration_points);

        Index j = 0;

        for (Index i = 0; i < tessellation.nb_triangles(); i++) {
            const auto [a, b, c] = tessellation.triangle(i);

            const Vector2d vertex_a = polygon.vertex(a);
            const Vector2d vertex_b = polygon.vertex(b);
            const Vector2d vertex_c = polygon.vertex(c);

            const Vector2d ab = vertex_b - vertex_a;
            const Vector2d ac = vertex_c - vertex_a;

            const double area = 0.5 * norm(cross(ab, ac));

            for (const auto& norm_point : xiao_gimbutas) {
                const auto uv = vertex_a * std::get<0>(norm_point) +
                                vertex_b * std::get<1>(norm_point) +
                                vertex_c * std::get<2>(norm_point);

                integration_points[j++] = IntegrationPoint<2>(uv[0], uv[1],
                    area * std::get<3>(norm_point));
            }
        }

        return integration_points;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;
        
        using Type = PolygonIntegrationPoints;

        m.def("integration_points", [](const Index degree, const Polygon& polygon)
            { return Type::get(degree, polygon); }, "degree"_a, "polygon"_a);
    }
};

} // namespace anurbs
