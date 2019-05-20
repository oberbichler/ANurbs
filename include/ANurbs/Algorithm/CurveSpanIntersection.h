#pragma once

#include "../Define.h"

#include "../Geometry/CurveBase.h"
#include "CurveTessellation.h"

#include <array>
#include <limits>
#include <utility>

namespace ANurbs {

class CurveSpanIntersection
{
public:     // types
    using CurveBase = CurveBase<2>;
    using Vector = Vector<2>;
    using ParameterPoint = std::pair<double, Vector>;

private:    // types
    struct Axis
    {
        int m_index;
        std::vector<double> m_values;
        double m_tolerance;

        inline double value(const Vector& point) const
        {
            return point[m_index];
        }

        void initialize(const int axis, const std::vector<double>& knots,
            double tolerance)
        {
            m_index = axis;

            m_values.clear();

            double nInf = std::numeric_limits<double>::lowest();
            double pInf = std::numeric_limits<double>::max();

            m_values.push_back(nInf);

            for (double knot : knots) {
                if (std::abs(m_values.back() - knot) > tolerance) {
                    m_values.push_back(knot);
                }
            }

            m_values.push_back(pInf);

            m_tolerance = tolerance;
        }

        void intersect(const CurveBase& curve, const ParameterPoint& a,
            const ParameterPoint& b, std::vector<double>& parameters)
        {
            double tA = std::get<0>(a);
            double valueA = value(std::get<1>(a));

            double tB = std::get<0>(b);
            double valueB = value(std::get<1>(b));
            
            // make sure that valueA <= valueB
            if (valueA > valueB) {
                std::swap(valueA, valueB);
                std::swap(tA, tB);
            }
            
            // index of the first intersect value
            std::size_t indexA;
            {
                auto it = std::lower_bound(std::begin(m_values),
                    std::end(m_values), valueA - m_tolerance);
                indexA = std::distance(std::begin(m_values), it);
            }

            // index of the first non intersect value
            std::size_t indexB;
            {
                auto it = std::upper_bound(std::begin(m_values),
                    std::end(m_values), valueB + m_tolerance);
                indexB = std::distance(std::begin(m_values), it);
            }

            // find intersections
            for (std::size_t i = indexA; i < indexB; i++) {
                double target = m_values[i];
            
                double t = tA;

                double delta = valueA - valueB;

                if (delta != 0) {
                    t += (valueA - target) / delta * (tB - tA);
                }

                for (int j = 0; j < 100; j++) {
                    auto c = curve.derivatives_at(t, 1);

                    double f = value(c[0]) - target;
                    
                    if (std::abs(f) < m_tolerance) {
                        break;
                    }

                    double df = value(c[1]);

                    if (df != 0) {
                        t -= f / df;
                    } else {
                        break;
                    }
                }

                // FIXME: check for convergency

                if (curve.domain().contains(t)) {
                    parameters.push_back(t);
                }
            }
        }
    };

private:    // static methods
    template <typename TContainer>
    static void unique_sorted(TContainer& container,
        const double& tolerance = 1e-7)
    {
        std::sort(std::begin(container), std::end(container));

        auto last = std::unique(std::begin(container), std::end(container),
            [=](double a, double b) { return b - a < tolerance; });

        auto nb_unique = std::distance(std::begin(container), last);

        container.resize(nb_unique);
    }

public:     // static methods
    static std::vector<double> compute(const CurveBase& curve,
        const std::vector<double>& knots_u, const std::vector<double>& knots_v,
        const double tolerance, const bool include_curve_knots)
    {
        std::vector<double> intersection_parameters;

        // approximate curve with a polyline

        const auto tessellation =
            CurveTessellation<2>::compute(curve, tolerance);

        // initialize axes

        std::array<Axis, 2> axes;

        axes[0].initialize(0, knots_u, tolerance);
        axes[1].initialize(1, knots_v, tolerance);

        // add curve knots

        if (include_curve_knots) {
            double t0 = tessellation[0].first;

            intersection_parameters.push_back(t0);

            for (const auto& span : curve.spans()) {
                intersection_parameters.push_back(span.t1());
            }
        }

        // check line segments

        for (int i = 1; i < tessellation.size(); i++) {
            const auto a = tessellation[i - 1];
            const auto b = tessellation[i];

            for (auto& axis : axes) {
                axis.intersect(curve, a, b, intersection_parameters);
            }
        }

        unique_sorted(intersection_parameters);

        return intersection_parameters;
    }

public:     // python
    template <typename TModule>
    static void register_python(TModule& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveSpanIntersection;

        m.def("curve_span_intersection", &Type::compute, "curve"_a, "knots_u"_a,
            "knots_v"_a, "tolerance"_a, "include_curve_knots"_a);
    }
};

} // namespace ANurbs
