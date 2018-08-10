#pragma once

#include "Curve.h"
#include "CurveBase.h"
#include "CurveGeometry.h"
#include "CurveSpanIntersection.h"
#include "Pointer.h"
#include "SurfaceGeometry.h"

namespace ANurbs {

template <typename TCurveGeometry, typename TSurfaceGeometry>
class CurveOnSurface
    : public CurveBase<
          typename TCurveGeometry::ScalarType,
          typename TSurfaceGeometry::VectorType>
{
public:
    using CurveBaseType = CurveBase<
        typename TCurveGeometry::ScalarType,
        typename TSurfaceGeometry::VectorType>;

    using CurveGeometryType = TCurveGeometry;
    using SurfaceGeometryType = TSurfaceGeometry;

    using ScalarType = typename CurveBaseType::ScalarType;
    using VectorType = typename SurfaceGeometryType::VectorType;
    using IntervalType = typename CurveBaseType::IntervalType;

private:
    Pointer<CurveGeometryType> m_curveGeometry;
    Pointer<SurfaceGeometryType> m_surfaceGeometry;
    const IntervalType m_domain;

public:
    CurveOnSurface(
        Pointer<CurveGeometryType> curveGeometry,
        Pointer<SurfaceGeometryType> surfaceGeometry,
        IntervalType domain
    )
    : m_curveGeometry(curveGeometry)
    , m_surfaceGeometry(surfaceGeometry)
    , m_domain(domain)
    {
        static_assert(TCurveGeometry::Dimension() == 2, "Not a planar curve");
        static_assert(std::is_same<typename TCurveGeometry::ScalarType,
            typename TSurfaceGeometry::ScalarType>::value,
            "Different scalar types");
    }

    Pointer<CurveGeometryType>
    CurveGeometry(
    ) const
    {
        return m_curveGeometry;
    }

    Pointer<SurfaceGeometryType>
    SurfaceGeometry() const
    {
        return m_surfaceGeometry;
    }

    int
    Degree() const override
    {
        return std::max({m_curveGeometry->Degree(), m_surfaceGeometry->DegreeU()
            , m_surfaceGeometry->DegreeV()});
    }

    IntervalType
    Domain() const override
    {
        return m_domain;
    }

    VectorType
    PointAt(
        const ScalarType t
    ) const override
    {
        auto uv = m_curveGeometry->PointAt(t);

        auto point = m_surfaceGeometry->PointAt(uv[0], uv[1]);

        return point;
    }

    std::vector<VectorType>
    DerivativesAt(
        const ScalarType t,
        const int order
    ) const override
    {
        // derivatives of base geometries

        auto curveDerivatives = m_curveGeometry->DerivativesAt(t, order);

        ScalarType u = curveDerivatives[0][0];
        ScalarType v = curveDerivatives[0][1];

        auto surfaceDerivatives = m_surfaceGeometry->DerivativesAt(u, v, order);

        // compute combined derivatives

        std::vector<VectorType> derivatives(order + 1);

        std::function<VectorType(int, int, int)> c;

        c = [&](int order, int i, int j) -> VectorType {
            if (order > 0) {
                VectorType result = Internals::Zero<VectorType>::get();

                for (int a = 1; a <= order; a++) {
                    result += (
                        c(order - a, i + 1, j) * curveDerivatives[a][0] +
                        c(order - a, i, j + 1) * curveDerivatives[a][1]
                    ) * Math::Binom(order - 1, a - 1);
                }

                return result;
            } else {
                int index = SurfaceShapeEvaluator<double>::ShapeIndex(i, j);
                return surfaceDerivatives[index];
            }
        };

        for (int i = 0; i <= order; i++) {
            derivatives[i] = c(i, 0, 0);
        }

        return derivatives;
    }

    std::vector<IntervalType>
    Spans() const override
    {
        using Vector2Type = typename CurveGeometryType::VectorType;

        auto curve = Create<Curve<CurveGeometryType>>(CurveGeometry(),
            Domain());

        auto knotsU = SurfaceGeometry()->KnotsU();
        auto knotsV = SurfaceGeometry()->KnotsV();

        CurveSpanIntersection<ScalarType, Vector2Type> intersection;
        intersection.Compute(*curve, knotsU, knotsV, 1e-4, true);

        int nbSpans = intersection.NbIntersections() - 1;

        std::vector<IntervalType> result(nbSpans);

        for (int i = 0; i < nbSpans; i++) {
            ScalarType t0 = intersection.Parameter(i);
            ScalarType t1 = intersection.Parameter(i + 1);

            result[i] = IntervalType(t0, t1);
        }

        return result;
    }
};

using CurveOnSurface1D = CurveOnSurface<CurveGeometry2D, SurfaceGeometry1D>;
using CurveOnSurface2D = CurveOnSurface<CurveGeometry2D, SurfaceGeometry2D>;
using CurveOnSurface3D = CurveOnSurface<CurveGeometry2D, SurfaceGeometry3D>;

} // namespace ANurbs
