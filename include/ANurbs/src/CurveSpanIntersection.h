#pragma once

#include "CurveBase.h"
#include "CurveGeometry.h"
#include "CurveTessellation.h"
#include "SurfaceGeometry.h"
#include "Pointer.h"

#include <array>
#include <utility>

namespace ANurbs {

template <typename TScalar, typename TVector>
class CurveSpanIntersection
{
public:
    using ScalarType = TScalar;
    using VectorType = TVector;

    using CurveType = CurveBase<ScalarType, VectorType>;

private:
    using ParameterPoint = std::pair<ScalarType, VectorType>;

private:
    std::vector<ScalarType> m_intersectionParameters;

public:
    CurveSpanIntersection(
        Pointer<CurveType> curve,
        std::vector<ScalarType> knotsU,
        std::vector<ScalarType> knotsV,
        const double tolerance = 1e-3,
        const bool includeCurveKnots = false)
    {
        static_assert(CurveType::Dimension() == 2,
            "Only planar curves are supported");

        Compute(curve, knotsU, knotsV, tolerance, includeCurveKnots);
    }

    int
    NbIntersections() const
    {
        return static_cast<int>(m_intersectionParameters.size());
    }

    ScalarType
    Parameter(
        const int& index) const
    {
        return m_intersectionParameters[index];
    }

private:
    struct Axis
    {
        std::vector<ScalarType> m_knots;
        int m_position;
        int m_axis;
        ScalarType m_parameter;

        inline ScalarType
        GetValue(
            const VectorType& point) const
        {
            return point[m_axis];
        }

        void Initialize(
            const int axis,
            const std::vector<ScalarType>& knots,
            const ScalarType& parameterAtStart)
        {
            m_knots.clear();

            m_axis = axis;

            ScalarType mInf = std::numeric_limits<ScalarType>::lowest();
            ScalarType pInf = std::numeric_limits<ScalarType>::max();

            m_knots.push_back(mInf);

            for (ScalarType knot : knots) {
                if (std::abs(m_knots.back() - knot) > 1e-7) {
                    m_knots.push_back(knot);
                }
            }

            m_knots.push_back(pInf);

            for (int i = 0; i < m_knots.size(); i++) {
                if (parameterAtStart < m_knots[i]) {
                    m_position = i - 1;
                    break;
                }
            }
        }

        ScalarType
        Lower() const
        {
            return m_knots[m_position];
        }

        ScalarType
        Upper() const
        {
            return m_knots[m_position + 1];
        }

        bool
        Intersect(
            const Pointer<CurveType> curve,
            const ParameterPoint& a,
            const ParameterPoint& b,
            const ScalarType tolerance)
        {
            ScalarType t0 = std::get<0>(a);
            ScalarType v0 = GetValue(std::get<1>(a));

            ScalarType t1 = std::get<0>(b);
            ScalarType v1 = GetValue(std::get<1>(b));

            ScalarType target;

            if (v1 > Upper() - tolerance) {
                target = Upper();
                if (v1 > Upper()) {
                    m_position += 1;
                }
            } else if (v1 < Lower() + tolerance) {
                target = Lower();
                if (v1 < Lower()) {
                    m_position -= 1;
                }
            } else {
                return false;
            }

            ScalarType t = t0;

            ScalarType delta = v0 - v1;

            if (delta != 0) {
                t += (v0 - target) / delta * (t1 - t0);
            }

            for (int j = 0; j < 100; j++) {
                auto c = curve->DerivativesAt(t, 1);

                ScalarType f = c[0][m_axis] - target;
                ScalarType df = c[1][m_axis];

                if (df != 0) {
                    t -= f / df;
                }

                if (std::abs(df) < 1e-7) {
                    m_parameter = t;
                    return true;
                }
            }

            m_parameter = t;

            // FIXME: not converged

            return true;
        }

        ScalarType
        Parameter() const
        {
            return m_parameter;
        }
    };

    template <typename TContainer>
    static void
    UniqueSorted(
        TContainer& container,
        const ScalarType& tolerance = 1e-7)
    {
        std::sort(std::begin(container), std::end(container));

        auto last = std::unique(std::begin(container), std::end(container),
            [=](ScalarType a, ScalarType b) -> bool {
                return b - a < tolerance;
            });

        auto nbUnique = std::distance(std::begin(container), last);

        container.resize(nbUnique);
    }

    void
    Compute(
        Pointer<CurveType> curve,
        const std::vector<ScalarType>& knotsU,
        const std::vector<ScalarType>& knotsV,
        const ScalarType tolerance = 1e-3,
        const bool includeCurveKnots = false)
    {
        // approximate curve with a polyline

        CurveTessellation2D tessellation;

        tessellation.Compute(*curve, tolerance);

        // start parameter and point

        ScalarType t0 = tessellation.Parameter(0);
        ScalarType u0 = tessellation.Point(0)[0];
        ScalarType v0 = tessellation.Point(0)[1];

        // initialize axes

        std::array<Axis, 2> axes;

        axes[0].Initialize(0, knotsU, u0);
        axes[1].Initialize(1, knotsV, v0);

        // check start point

        if ((std::abs(u0 - axes[0].Lower()) < tolerance) ||
            (std::abs(u0 - axes[0].Upper()) < tolerance) ||
            (std::abs(v0 - axes[1].Lower()) < tolerance) ||
            (std::abs(v0 - axes[1].Upper()) < tolerance)) {
            m_intersectionParameters.push_back(t0);
        }

        // add curve knots

        if (includeCurveKnots) {
            m_intersectionParameters.push_back(t0);

            for (const auto& span : curve->Spans()) {
                m_intersectionParameters.push_back(span.T1());
            }
        }

        // check line segments

        for (int i = 1; i < tessellation.NbPoints(); i++) {
            ParameterPoint a = {
                tessellation.Parameter(i - 1),
                tessellation.Point(i - 1)
            };

            ParameterPoint b = {
                tessellation.Parameter(i - 0),
                tessellation.Point(i - 0)
            };

            for (auto& axis : axes) {
                if (axis.Intersect(curve, a, b, tolerance)) {
                    ScalarType t = axis.Parameter();
                    m_intersectionParameters.push_back(t);
                }
            }
        }

        UniqueSorted(m_intersectionParameters);
    }
};

using CurveSpanIntersection2D = CurveSpanIntersection<double, Point2D>;

} // namespace ANurbs
