#pragma once

#include "../Define.h"

#include <algorithm>

namespace anurbs {

class Nurbs
{
public:     // static methods
    template <typename TKnots>
    static Index upper_bound(TKnots values, Index first, Index last, double value)
    {
        auto count = last - first;
        Index i, step;
    
        while (count > 0) {
            i = first; 
            step = count / 2; 
            i += step;
            if (!(value < values[i])) {
                first = ++i;
                count -= step + 1;
            } else {
                count = step;
            }
        }
        return first;
    }

    template <typename TKnots>
    static Index lower_bound(TKnots values, Index first, Index last, double value)
    {
        auto count = last - first;
        Index i, step;
    
        while (count > 0) {
            i = first; 
            step = count / 2; 
            i += step;
            if (values[i] < value) {
                first = ++i; 
                count -= step + 1; 
            } else {
                count = step;
            }
        }
        return first;
    }

    template <typename TKnots>
    static Index upper_span(const Index degree, const TKnots& knots, const double& t)
    {
        return upper_bound(knots, degree, length(knots) - degree, t) - 1;
    }

    template <typename TKnots>
    static Index lower_span(const Index degree, const TKnots& knots, const double& t)
    {
        return lower_bound(knots, degree, length(knots) - degree, t) - 1;
    }

    static Index degree(const Index nb_knots, const Index nb_poles)
    {
        return nb_knots - nb_poles + 1;
    }

    static Index nb_knots(const Index degree, const Index nb_poles)
    {
        return nb_poles + degree - 1;
    }

    static Index nb_poles(const Index degree, const Index nb_knots)
    {
        return nb_knots - degree + 1;
    }

    static Index nb_spans(const Index degree, const Index nb_knots)
    {
        return nb_knots - 2 * degree + 1;
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Nurbs;

        m.def("upper_span", &Type::upper_span<std::vector<double>>);
        m.def("lower_span", &Type::lower_span<std::vector<double>>);
        m.def("degree", &Type::degree);
        m.def("nb_knots", &Type::nb_knots);
        m.def("nb_poles", &Type::nb_poles);
        m.def("nb_spans", &Type::nb_spans);
    }
};

} // namespace anurbs