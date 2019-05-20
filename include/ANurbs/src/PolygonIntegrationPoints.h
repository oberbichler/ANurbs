#pragma once

#include "../Define.h"

#include "IntegrationPoints.h"

#include "../Algorithm/PolygonTessellation.h"
#include "../Geometry/Interval.h"
#include "../Geometry/Polygon.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

class PolygonIntegrationPoints
{
public:
    using IntegrationPoint2Type = IntegrationPoint2;
    using IntegrationPointBarycentricType = IntegrationPointBarycentric;

private:
    std::vector<IntegrationPoint2Type> m_integrationPoints;
    PolygonTessellation m_tessellation;

public:
    void Compute(const size_t degree, const Polygon& polygon)
    {
        m_tessellation.compute(polygon);

        const auto& xiaogimb = IntegrationPoints::XiaoGimbutas(degree);

        const int nbIntegrationPoints =
            m_tessellation.nb_triangles() * static_cast<int>(xiaogimb.size());

        m_integrationPoints.resize(nbIntegrationPoints);

        auto integrationPoint = m_integrationPoints.begin();

        for (int i = 0; i < m_tessellation.nb_triangles(); i++) {
            const auto [a, b, c] = m_tessellation.triangle(i);

            const Vector<2> vertexA = polygon.vertex(a);
            const Vector<2> vertexB = polygon.vertex(b);
            const Vector<2> vertexC = polygon.vertex(c);

            const Vector<2> vectorAB = vertexB - vertexA;
            const Vector<2> vectorAC = vertexC - vertexA;

            const double area = 0.5 * norm(cross(vectorAB, vectorAC));

            for (const auto& normalizedPoint : xiaogimb) {
                const auto uv = vertexA * normalizedPoint.a +
                    vertexB * normalizedPoint.b + vertexC * normalizedPoint.c;

                integrationPoint->u = uv[0];
                integrationPoint->v = uv[1];
                integrationPoint->weight = area * normalizedPoint.weight;
                integrationPoint++;
            }
        }
    }

    int NbIntegrationPoints() const
    {
        return static_cast<int>(m_integrationPoints.size());
    }

    IntegrationPoint2Type IntegrationPoint(const int index) const
    {
        return m_integrationPoints[index];
    }
};

} // namespace ANurbs
