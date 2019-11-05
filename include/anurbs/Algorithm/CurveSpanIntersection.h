#pragma once

#include "../Define.h"

#include "CurveTessellation.h"

#include "../Geometry/CurveBase.h"

#include <array>
#include <limits>
#include <utility>

namespace anurbs {

class CurveSpanIntersection
{
public:     // types
    using Vector = linear_algebra::Vector<2>;
    using ParameterPoint = std::pair<double, Vector>;

private:    // types
    struct Axis
    {
        Index m_index;
        std::vector<double> m_values;
        double m_tolerance;

        inline double value(const Vector& point) const
        {
            return point[m_index];
        }

        void initialize(const Index axis, const std::vector<double>& knots,
            double tolerance)
        {
            m_index = axis;

            m_values.clear();

            double n_inf = std::numeric_limits<double>::lowest();
            double p_inf = std::numeric_limits<double>::max();

            m_values.push_back(n_inf);

            for (const double knot : knots) {
                if (std::abs(m_values.back() - knot) > tolerance) {
                    m_values.push_back(knot);
                }
            }

            m_values.push_back(p_inf);

            m_tolerance = tolerance;
        }

        void intersect(const CurveBase<2>& curve, const ParameterPoint& a,
            const ParameterPoint& b, std::vector<double>& parameters)
        {
            double t_a = std::get<0>(a);
            double value_a = value(std::get<1>(a));

            double t_b = std::get<0>(b);
            double value_b = value(std::get<1>(b));
            
            // make sure that value_a <= value_b
            if (value_a > value_b) {
                std::swap(value_a, value_b);
                std::swap(t_a, t_b);
            }
            
            // index of the first intersect value
            size_t index_a;
            {
                auto it = std::lower_bound(std::begin(m_values),
                    std::end(m_values), value_a - m_tolerance);
                index_a = std::distance(std::begin(m_values), it);
            }

            // index of the first non intersect value
            size_t index_b;
            {
                auto it = std::upper_bound(std::begin(m_values),
                    std::end(m_values), value_b + m_tolerance);
                index_b = std::distance(std::begin(m_values), it);
            }

            // find intersections
            for (size_t i = index_a; i < index_b; i++) {
                double target = m_values[i];
            
                double t = t_a;

                double delta = value_a - value_b;

                if (delta != 0) {
                    t += (value_a - target) / delta * (t_b - t_a);
                }

                for (Index j = 0; j < 100; j++) {
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
    static std::vector<double> compute(const CurveBase<2>& curve,
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

        for (Index i = 1; i < length(tessellation); i++) {
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
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveSpanIntersection;

        m.def("curve_span_intersection", &Type::compute, "curve"_a, "knots_u"_a,
            "knots_v"_a, "tolerance"_a, "include_curve_knots"_a);
    }
};

} // namespace anurbs
