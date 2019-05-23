#pragma once

#include "../Define.h"

#include <algorithm>

namespace ANurbs {

class KnotVector
{
public:     // static methods
    template <typename TKnots>
    static int upper_span(const int degree, const TKnots& knots,
        const double& t)
    {
        auto span = std::upper_bound(std::begin(knots) + degree, std::end(knots)
            - degree, t) - std::begin(knots) - 1;
        return static_cast<int>(span);
    }

    template <typename TKnots>
    static int lower_span(const int degree, const TKnots& knots,
        const double& t)
    {
        auto span = std::lower_bound(std::begin(knots) + degree, std::end(knots)
            - degree, t) - std::begin(knots) - 1;
        return static_cast<int>(span);
    }

    static int degree(const int nb_knots, const int nb_poles)
    {
        return nb_knots - nb_poles + 1;
    }

    static int nb_knots(const int degree, const int nb_poles)
    {
        return nb_poles + degree - 1;
    }

    static int nb_poles(const int degree, const int nb_knots)
    {
        return nb_knots - degree + 1;
    }

    static int nb_spans(const int degree, const int nb_knots)
    {
        return nb_knots - 2 * degree + 1;
    }

public:     // python

    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = KnotVector;

        m.def("upper_span", &Type::upper_span<std::vector<double>>);
        m.def("lower_span", &Type::lower_span<std::vector<double>>);
        m.def("degree", &Type::degree);
        m.def("nb_knots", &Type::nb_knots);
        m.def("nb_poles", &Type::nb_poles);
        m.def("nb_spans", &Type::nb_spans);
    }
};

} // namespace ANurbs