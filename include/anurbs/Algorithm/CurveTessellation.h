#pragma once

#include "../Define.h"

#include "../Geometry/CurveBase.h"

namespace anurbs {

template <Index TDimension>
class CurveTessellation {
public: // types
    using CurveBaseD = CurveBase<TDimension>;
    using Vector = Eigen::Matrix<double, 1, TDimension>;
    using ParameterPoint = std::pair<double, Vector>;

private: // static methods
    static double distance_to_line(const Vector& point,
        const Vector& line_a, const Vector& line_b)
    {
        Vector v = line_a - point;
        Vector u = line_b - line_a;

        return norm(cross(v, u)) / norm(u);
    }

public: // static methods
    static std::pair<std::vector<double>, std::vector<Vector>> compute(const CurveBaseD& curve, const double tolerance)
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

        sample_points.emplace_back(1.0, curve.point_at(domain.parameter_at_normalized(1.0)));

        std::sort(std::begin(sample_points), std::end(sample_points),
            [](auto const& lhs, auto const& rhs) {
                return std::get<0>(lhs) > std::get<0>(rhs);
            });

        // compute polyline

        const Index n = curve.degree() * 2 + 1;

        while (true) {
            const auto [t_a, point_a] = sample_points.back();
            sample_points.pop_back();

            points.emplace_back(domain.parameter_at_normalized(t_a), point_a);

            if (sample_points.empty()) {
                break;
            }

            while (true) {
                const auto [t_b, point_b] = sample_points.back();

                double max_distance {0};
                ParameterPoint max_point;

                for (Index i = 1; i <= n; i++) {
                    const double t = Interval::parameter_at_normalized(t_a, t_b, i / double(n + 1));
                    const Vector point = curve.point_at(domain.parameter_at_normalized(t));

                    const double distance = distance_to_line(point, point_a, point_b);

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

        std::vector<double> parameters(points.size());
        std::vector<Vector> locations(points.size());

        for (Index i = 0; i < length(points); i++) {
            parameters[i] = std::get<0>(points[i]);
            locations[i] = std::get<1>(points[i]);
        }

        return {parameters, locations};
    }

public: // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = CurveTessellation<TDimension>;

        m.def("tessellate", &Type::compute, "curve"_a, "tolerance"_a = 1e-4);
    }
};

} // namespace anurbs