#pragma once

#include "CurveBase.h"

namespace ANurbs {

template <typename TScalar, typename TVector>
class CurveTessellation
{
public:
    using CurveBaseType = CurveBase<TScalar, TVector>;

    using ScalarType = TScalar;
    using VectorType = TVector;
    using IntervalType = Interval<ScalarType>;

private:
    struct ParameterPoint
    {
        ScalarType t;
        VectorType point;

        ParameterPoint()
        {
        }

        ParameterPoint(
            ScalarType t,
            VectorType point)
        {
            this->t = t;
            this->point = point;
        }

        bool
        operator>(
            const ParameterPoint& other) const
        {
            return t > other.t;
        }
    };

    std::vector<ParameterPoint> m_samplePoints;
    std::vector<ParameterPoint> m_points;

public:
    CurveTessellation()
    {
    }

    ScalarType
    DistancePointToLine(
        const VectorType& point,
        const VectorType& lineA,
        const VectorType& lineB
    )
    {
        VectorType v = lineA - point;
        VectorType u = lineB - lineA;

        return VectorMath<TVector>::Norm(VectorMath<TVector>::Cross(v, u)) /
            VectorMath<TVector>::Norm(u);
    }

    void
    Compute(
        const CurveBaseType& curve,
        const ScalarType flatness
    )
    {
        m_samplePoints.clear();
        m_points.clear();

        IntervalType& domain = curve.Domain();

        // compute sample points

        for (const auto& span : curve.Spans()) {
            IntervalType normalizedSpan = domain.NormalizedInterval(span);

            if (normalizedSpan.Length() < 1e-7) {
                continue;
            }

            m_samplePoints.emplace_back(normalizedSpan.T0(),
                curve.PointAt(span.T0()));
        }

        m_samplePoints.emplace_back(1.0,
            curve.PointAt(domain.ParameterAtNormalized(1.0)));

        std::sort(std::begin(m_samplePoints), std::end(m_samplePoints),
            std::greater<ParameterPoint>());

        // compute polyline

        int n = curve.Degree() * 2 + 1;

        while (true) {
            ParameterPoint a = m_samplePoints.back();
            m_samplePoints.pop_back();

            m_points.emplace_back(domain.ParameterAtNormalized(a.t), a.point);

            if (m_samplePoints.size() == 0) {
                break;
            }

            while (true) {
                ParameterPoint b = m_samplePoints.back();

                ScalarType maxDistance {0};
                ParameterPoint maxPoint;

                for (int i = 1; i <= n; i++) {
                    ScalarType t = IntervalType::ParameterAtNormalized(a.t, b.t,
                        i / ScalarType(n + 1));
                    VectorType point = curve.PointAt(
                        domain.ParameterAtNormalized(t));

                    ScalarType distance = DistancePointToLine(point, a.point,
                        b.point);

                    if (distance > maxDistance) {
                        maxDistance = distance;
                        maxPoint = {t, point};
                    }
                }

                if (maxDistance < flatness) {
                    break;
                }

                m_samplePoints.push_back(maxPoint);
            }
        }
    }

    int
    NbPoints() const
    {
        return static_cast<int>(m_points.size());
    }

    VectorType
    Parameter(
        const int index) const
    {
        return m_points.at(index).parameter;
    }

    VectorType
    Point(
        const int index) const
    {
        return m_points.at(index).point;
    }
};

using CurveTessellation2D = CurveTessellation<double, Point<double, 2>>;
using CurveTessellation3D = CurveTessellation<double, Point<double, 3>>;

} // namespace ANurbs