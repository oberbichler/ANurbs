#pragma once

#include "../Define.h"

#include "../Geometry/CurveBase.h"

namespace ANurbs {

template <int TDimension>
class CurveTessellation
{
public:     // types
    using CurveBase = CurveBase<TDimension>;
    using Vector = Vector<TDimension>;
    using ParameterPoint = std::pair<double, Vector>;

private:    // static methods
    static double distance_to_line(const Vector& point,
        const Vector& line_a, const Vector& line_b)
    {
        Vector v = line_a - point;
        Vector u = line_b - line_a;

        return norm(cross(v, u)) / norm(u);
    }

public:     // static methods
    static std::vector<ParameterPoint> compute(const CurveBase& curve,
        const double tolerance)
    {
        const Interval domain = curve.domain();

        std::vector<ParameterPoint> sample_points;
        std::vector<ParameterPoint> points;

        // compute sample points

        for (const auto& span : curve.spans()) {
            const Interval normalized_span = domain.normalized_interval(span);

            if (normalized_span.length() < 1e-7) {
                continue;
            }

            const double t = normalized_span.t0();
            const Vector point = curve.point_at(span.t0());

            sample_points.emplace_back(t, point);
        }

        sample_points.emplace_back(1.0,
            curve.point_at(domain.parameter_at_normalized(1.0)));

        std::sort(std::begin(sample_points), std::end(sample_points),
            [](auto const &lhs, auto const &rhs) {
                return std::get<0>(lhs) > std::get<0>(rhs);
            }
        );

        // compute polyline

        const int n = curve.degree() * 2 + 1;

        while (true) {
            const auto [t_a, point_a] = sample_points.back();
            sample_points.pop_back();

            points.emplace_back(domain.parameter_at_normalized(t_a), point_a);

            if (sample_points.size() == 0) {
                break;
            }

            while (true) {
                const auto [t_b, point_b] = sample_points.back();

                double max_distance {0};
                ParameterPoint max_point;

                for (int i = 1; i <= n; i++) {
                    const double t = Interval::parameter_at_normalized(t_a, t_b,
                        i / double(n + 1));
                    const Vector point = curve.point_at(
                        domain.parameter_at_normalized(t));

                    const double distance = distance_to_line(point, point_a,
                        point_b);

                    if (distance > max_distance) {
                        max_distance = distance;
                        max_point = {t, point};
                    }
                }

                if (max_distance < tolerance) {
                    break;
                }

                sample_points.push_back(max_point);
            }
        }

        return points;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveTessellation<TDimension>;
        
        m.def("tessellate", &Type::compute, "curve"_a, "tolerance"_a=1e-4);
    }
};

} // namespace ANurbs