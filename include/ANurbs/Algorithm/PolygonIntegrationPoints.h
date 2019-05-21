#pragma once

#include "../Define.h"

#include "IntegrationPoints.h"
#include "PolygonTessellation.h"

#include "../Geometry/Interval.h"
#include "../Geometry/Polygon.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

class PolygonIntegrationPoints
{
private:
    IntegrationPointList<2> m_integrationPoints;
    PolygonTessellation m_tessellation;

public:
    void Compute(const size_t degree, const Polygon& polygon)
    {
        m_tessellation.compute(polygon);

        const auto& xiao_gimbutas = IntegrationPoints::xiao_gimbutas(degree);

        const int nb_integration_points = m_tessellation.nb_triangles() *
            static_cast<int>(xiao_gimbutas.size());

        m_integrationPoints.resize(nb_integration_points);

        int j = 0;

        for (int i = 0; i < m_tessellation.nb_triangles(); i++) {
            const auto [a, b, c] = m_tessellation.triangle(i);

            const Vector<2> vertex_a = polygon.vertex(a);
            const Vector<2> vertex_b = polygon.vertex(b);
            const Vector<2> vertex_c = polygon.vertex(c);

            const Vector<2> ab = vertex_b - vertex_a;
            const Vector<2> ac = vertex_c - vertex_a;

            const double area = 0.5 * norm(cross(ab, ac));

            for (const auto& norm_point : xiao_gimbutas) {
                const auto uv = vertex_a * norm_point[0] + vertex_b *
                    norm_point[1] + vertex_c * norm_point[2];

                m_integrationPoints[j] = IntegrationPoint<2>(uv[0], uv[1],
                    area * norm_point[3]);
            }
        }
    }

    int nb_integration_points() const
    {
        return static_cast<int>(m_integrationPoints.size());
    }

    IntegrationPoint<2> integration_point(const int index) const
    {
        return m_integrationPoints[index];
    }

public:     // python
    template <typename TModule>
    static void register_python(TModule& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;
        
        using Type = PolygonIntegrationPoints;

        py::class_<Type>(m, "PolygonIntegrationPoints")
        //     .def(py::init<>())
        //     .def("Compute", &Type::Compute, "degree"_a, "polygon"_a)
        //     .def("nb_integration_points", &Type::nb_integration_points)
        //     .def("IntegrationPoint", &Type::integration_point, "index"_a)
        ;
    }
};

} // namespace ANurbs
